#pragma once

#include <limits>
#include <functional>
#include <queue>
#include <cassert>

#include "impl/exceptions.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <template <class> class Adjacency, class G, class WM,
				class D = std::decay_t<std::result_of_t<const WM&(Edge<G>)>>,
				class Combine = std::plus<>, class Compare = std::less<>>
			Vert_map<G, std::pair<D, Edge<G>>>
			_dijkstra(const G& g, Vert<G> s, const WM& weight,
				const Combine& combine = {}, const Compare& compare = {},
				D zero = {}, D inf = std::numeric_limits<D>::max()) {
				using Verts = traits::Verts<G>;
				using Edges = traits::Edges<G>;
				auto closed = Verts::ephemeral_set(g);
				auto result = Verts::map(g,
					std::make_pair(inf, Edges::null(g)));
				using pair_type = std::pair<D, Vert<G>>;
				struct queue_compare {
					const Compare& compare;
					bool operator()(const pair_type& l, const pair_type& r) const {
						// arguments reversed because std::priority_queue is a max queue
						return compare(r.first, l.first);
					}
				};
				std::priority_queue<pair_type, std::vector<pair_type>, queue_compare> queue(queue_compare{compare});
				result[s].first = zero;
				queue.emplace(zero, s);
				while (!queue.empty()) {
					auto[d, v] = queue.top();
					queue.pop();
					if (!closed.insert(v))
						continue;
					for (auto e : Adjacency<G>::range(g, v)) {
						auto u = traits::trait_cokey<Adjacency<G>>(g, e);
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
						decltype(auto) p = result[u];
						if (compare(c, p.first)) {
							assert(!closed.contains(u)); // sanity check which should never fail
							p = std::make_pair(c, e);
							queue.emplace(c, u);
						}
					}
				}
				return result;
			}
		}
		template <class Impl>
		template <class... Args>
		auto Out_edge_graph<Impl>::shortest_paths_from(Args&&... args) const {
			return impl::_dijkstra<impl::traits::Out_edges>(this->_impl(), std::forward<Args>(args)...);
		}
		template <class Impl>
		template <class... Args>
		auto In_edge_graph<Impl>::shortest_paths_to(Args&&... args) const {
			return impl::_dijkstra<impl::traits::In_edges>(this->_impl(), std::forward<Args>(args)...);
		}
	}
}
