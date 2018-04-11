#pragma once

#include "impl/Subforest.hpp"

#include <queue>

namespace graph {
	inline namespace v1 {
		template <class Impl>
		auto Graph<Impl>::out_subforest() const {
			using Subforest_impl = impl::Subforest<impl::traits::Out_edges<Impl>>;
			return Graph(Subforest_impl(this->_impl()));
		}
		template <class Impl>
		auto Graph<Impl>::in_subforest() const {
			using Subforest_impl = impl::Subforest<impl::traits::In_edges<Impl>>;
			return Graph(Subforest_impl(this->_impl()));
		}
		template <class Impl>
		auto Graph<Impl>::out_subtree(Vert root) const {
			using Subtree_impl = impl::Subtree<impl::traits::Out_edges<Impl>>;
			return Graph(Subtree_impl(this->_impl(), std::move(root)));
		}
		template <class Impl>
		auto Graph<Impl>::in_subtree(Vert root) const {
			using Subtree_impl = impl::Subtree<impl::traits::In_edges<Impl>>;
			return Graph(Subtree_impl(this->_impl(), std::move(root)));
		}
		namespace impl {
			// `Result_adjacency` determines whether the resulting tree spans vertices that can reach the vertex or can be reached _from_ the vertex.  This will generally by the reverse of the adjacency type supported by the underlying graph.
			template <template <class> class Result_adjacency, class G, class WM, class Compare>
			auto _prim(const G& g, const Vert<G>& v, const WM& weight, const Compare& compare) {
				using Verts = traits::Verts<G>;
				using Edges = traits::Edges<G>;
				using Graph_adjacencies = traits::Reverse_adjacency<Result_adjacency, G>;
				auto tree = Subtree<Result_adjacency<G>>(g, v);
				auto closed = Verts::ephemeral_set(g);
				using weight_type = std::decay_t<std::result_of_t<const WM&(typename Edges::value_type)>>;
				using pair_type = std::pair<weight_type, Edge<G>>;
				struct queue_compare {
					const Compare& compare;
					bool operator()(const pair_type& l, const pair_type& r) const {
						// arguments reversed because std::priority_queue is a max queue
						return compare(r.first, l.first);
					}
				};
				std::priority_queue<pair_type, std::vector<pair_type>, queue_compare> queue(queue_compare{compare});
				auto enqueue_edges = [&](typename Verts::value_type u){
					for (auto e : Graph_adjacencies::range(g, u))
						queue.emplace(weight(e), std::move(e));
				};
				closed.insert(v);
				enqueue_edges(v);
				while (!queue.empty()) {
					auto [_, e] = queue.top();
					queue.pop();
					auto v = traits::trait_cokey<Graph_adjacencies>(g, e);
					if (closed.insert(v)) {
						tree.insert_edge(e);
						enqueue_edges(v);
					}
				}
				return tree;
			}
		}
		template <class Impl>
		template <class WM, class Compare>
		auto Out_edge_graph<Impl>::minimum_tree_reachable_from(const Vert& s, const WM& weight, const Compare& compare) const {
			return impl::_prim<impl::traits::In_edges>(this->_impl(), s, weight, compare);
		}
		template <class Impl>
		template <class WM, class Compare>
		auto In_edge_graph<Impl>::minimum_tree_reaching_to(const Vert& t, const WM& weight, const Compare& compare) const {
			return impl::_prim<impl::traits::Out_edges>(this->_impl(), t, weight, compare);
		}
	}
}
