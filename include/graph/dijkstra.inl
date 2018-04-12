#pragma once

#include <limits>
#include <functional>
#include <queue>
#include <cassert>

#include "impl/exceptions.hpp"
#include "impl/Subforest.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class Adjacency, class G, class WM,
				class D = std::decay_t<std::result_of_t<const WM&(Edge<G>)>>,
				class Combine = std::plus<>, class Compare = std::less<>>
			std::pair<
				Subtree<traits::Reverse_adjacency<Adjacency>, G>,
				Vert_map<G, D>>
			_dijkstra(const G& g, Vert<G> s, const WM& weight,
				const Combine& combine = {}, const Compare& compare = {},
				D zero = {}, D inf = std::numeric_limits<D>::max()) {
				using Verts = traits::Verts<G>;
				using Edges = traits::Edges<G>;
				auto closed = Verts::ephemeral_set(g);
				auto tree = Subtree<traits::Reverse_adjacency<Adjacency>, G>(g, s);
				auto distances = Verts::map(g, inf);
				using pair_type = std::pair<D, Vert<G>>;
				struct queue_compare {
					const Compare& compare;
					bool operator()(const pair_type& l, const pair_type& r) const {
						// arguments reversed because std::priority_queue is a max queue
						return compare(r.first, l.first);
					}
				};
				std::priority_queue<pair_type, std::vector<pair_type>, queue_compare> queue(queue_compare{compare});
				distances[s] = zero;
				queue.emplace(zero, s);
				while (!queue.empty()) {
					auto [d, v] = queue.top();
					queue.pop();
					if (!closed.insert(v))
						continue;
					for (auto e : traits::Adjacent_edges<Adjacency, G>::range(g, v)) {
						auto u = traits::adjacency_cokey<Adjacency, G>(g, e);
#ifdef NDEBUG
						// optimization not required for correctness but skips checks below
						if (closed.contains(u))
							continue;
#endif
						auto c = combine(d, weight(e));
#ifndef NDEBUG
						if (compare(c, d))
							throw precondition_unmet("negative weight edge");
#endif
						decltype(auto) du = distances[u];
						if (compare(c, du)) {
							assert(!closed.contains(u)); // sanity check which should never fail
							du = c;
							tree.insert_edge(e); // replace the old edge in the tree
							queue.emplace(c, u);
						}
					}
				}
				return std::pair(tree, distances);
			}
		}
		template <class Impl>
		template <class... Args>
		auto Out_edge_graph<Impl>::shortest_paths_from(Args&&... args) const {
			auto [tree, distance] = impl::_dijkstra<impl::traits::Out>(this->_impl(), std::forward<Args>(args)...);
			return std::make_pair(Graph(std::move(tree)), std::move(distance));
		}
		template <class Impl>
		template <class... Args>
		auto In_edge_graph<Impl>::shortest_paths_to(Args&&... args) const {
			auto [tree, distance] = impl::_dijkstra<impl::traits::In>(this->_impl(), std::forward<Args>(args)...);
			return std::make_pair(Graph(std::move(tree)), std::move(distance));
		}
	}
}
