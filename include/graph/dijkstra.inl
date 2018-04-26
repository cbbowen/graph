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
			template <class Adjacency, class G, class Weight, class Compare, class Combine, class D>
			std::pair<
				Subtree<traits::Reverse_adjacency<Adjacency>, G>,
				Vert_map<G, D>>
			_dijkstra(const G& g, Vert<G> s, const Weight& weight,
				const Compare& compare, const Combine& combine,
				D zero, D inf) {
				using Verts = traits::Verts<G>;
				auto closed = Verts::ephemeral_set(g);
				auto tree = Subtree<traits::Reverse_adjacency<Adjacency>, G>(g, s);
				auto distance = Verts::map(g, inf);
				using pair_type = std::pair<D, Vert<G>>;
				struct queue_compare {
					const Compare& compare;
					bool operator()(const pair_type& l, const pair_type& r) const {
						// arguments reversed because std::priority_queue is a max queue
						return compare(r.first, l.first);
					}
				};
				std::priority_queue<pair_type, std::vector<pair_type>, queue_compare> queue(queue_compare{compare});
				distance[s] = zero;
				queue.emplace(zero, s);
				while (!queue.empty()) {
					auto [d, v] = queue.top();
					queue.pop();
					if (!closed.insert(v))
						continue;
					for (auto e : traits::Adjacent_edges<Adjacency, G>::range(g, v)) {
						auto u = traits::adjacency_cokey<Adjacency, G>(g, e);
#if !GRAPH_CHECK_PRECONDITIONS
						// optimization not required for correctness but skips checks below
						if (closed.contains(u))
							continue;
#endif
						auto c = combine(d, weight(e));
						check_precondition(!compare(c, d), "edges must have non-negative weights");

						decltype(auto) du = distance[u];
						if (compare(c, du)) {
							assert(!closed.contains(u)); // sanity check which should never fail
							du = c;
							tree.insert_edge(e); // replace the old edge in the tree
							queue.emplace(c, u);
						}
					}
				}
				return std::pair(std::move(tree), std::move(distance));
			}
		}
		template <class Impl>
		template <class Weight, class Compare, class Combine>
		auto Out_edge_graph<Impl>::shortest_paths_from(const Vert& s, const Weight& weight,
			const Compare& compare, const Combine& combine) const {
			// TODO: Convert these to parameters
			using D = std::decay_t<std::result_of_t<const Weight&(Edge)>>;
			auto zero = D{}, inf = std::numeric_limits<D>::infinity();

			auto [tree, distance] = impl::_dijkstra<impl::traits::Out>(this->_impl(), s, weight, compare, combine, zero, inf);
			return std::make_pair(_wrap_graph(std::move(tree)), std::move(distance));
		}
		template <class Impl>
		template <class Weight, class Compare, class Combine>
		auto In_edge_graph<Impl>::shortest_paths_to(const Vert& t, const Weight& weight,
			const Compare& compare, const Combine& combine) const {
			// TODO: Convert these to parameters
			using D = std::decay_t<std::result_of_t<const Weight&(Edge)>>;
			auto zero = D{}, inf = std::numeric_limits<D>::infinity();

			auto [tree, distance] = impl::_dijkstra<impl::traits::In>(this->_impl(), t, weight, compare, combine, zero, inf);
			return std::make_pair(_wrap_graph(std::move(tree)), std::move(distance));
		}
	}
}
