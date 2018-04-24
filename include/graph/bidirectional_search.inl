#pragma once

#include <limits>
#include <functional>
#include <queue>
#include <optional>
#include <algorithm>
#include <cassert>

#include <range/v3/algorithm/min.hpp>

#include "impl/exceptions.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class Adjacency, class G, class Queue,
				class Near, class Far,
				class Weight, class Distance, class Tree,
				class Compare, class Combine>
			bool _bidirectional_search_step(const G& g, Queue& queue,
				Near& near, const Far& far,
				const Weight& weight, Distance& distance, Tree& tree,
				const Compare& compare, const Combine& combine) {
				auto [d, v] = queue.top();
				queue.pop();
				if (near.insert(v)) {
					if (far.contains(v))
						return true;
					for (auto e : traits::Adjacent_edges<Adjacency, G>::range(g, v)) {
						auto u = traits::adjacency_cokey<Adjacency, G>(g, e);
#ifdef NDEBUG
						// optimization not required for correctness but skips checks below
						if (near.contains(u))
							continue;
#endif
						auto c = combine(d, weight(e));
#ifndef NDEBUG
						if (compare(c, d))
							throw precondition_unmet("negative weight edge");
#endif
						decltype(auto) du = distance[u];
						if (compare(c, du)) {
							assert(!near.contains(u)); // sanity check which should never fail
							du = c;
							tree.insert_edge(e); // replace the old edge in the tree
							queue.emplace(c, u);
						}
					}
				}
				return false;
			}
		}
		template <class Impl>
		template <class Weight, class Compare, class Combine>
		auto Bi_edge_graph<Impl>::shortest_path(const Vert& s, const Vert& t, const Weight& weight,
			const Compare& compare, const Combine& combine) const
			-> std::optional<std::vector<Edge>> {
			// TODO: Convert these to parameters
			using D = std::decay_t<std::result_of_t<const Weight&(Edge)>>;
			auto zero = D{}, inf = std::numeric_limits<D>::infinity();

			using pair_type = std::pair<D, Vert>;
			struct queue_compare {
				const Compare& compare;
				bool operator()(const pair_type& l, const pair_type& r) const {
					// arguments reversed because std::priority_queue is a max queue
					return compare(r.first, l.first);
				}
			};
			std::priority_queue<pair_type, std::vector<pair_type>, queue_compare>
				s_queue(queue_compare{ compare }), t_queue(queue_compare{ compare });

			auto s_closed = this->vert_set(), t_closed = this->vert_set();
			auto s_distance = this->vert_map(inf), t_distance = this->vert_map(inf);
			auto s_tree = this->in_subtree(s);
			auto t_tree = this->out_subtree(t);

			s_queue.emplace(s_distance[s] = zero, s);
			t_queue.emplace(t_distance[t] = zero, t);

			bool done = false;
			auto expand_s = [&] {
				done = impl::_bidirectional_search_step<impl::traits::Out>(this->_impl(), s_queue,
					s_closed, t_closed, weight, s_distance, s_tree, compare, combine);
			};
			auto expand_t = [&] {
				done = impl::_bidirectional_search_step<impl::traits::In>(this->_impl(), t_queue,
					t_closed, s_closed, weight, t_distance, t_tree, compare, combine);
			};

			// Interleave bidirectional search steps
			expand_s();
			expand_t();
			while (!done) {
				if (t_queue.size() < s_queue.size()) {
					if (t_queue.empty())
						return std::nullopt;
					expand_t();
				} else {
					if (s_queue.empty())
						return std::nullopt;
					expand_s();
				}
			}

			// Find the minimal rendezvous
			auto total_distance = [&](auto v) { return combine(s_distance(v), t_distance(v)); };
			auto rendezvous = ranges::min({
				ranges::min(s_closed, compare, total_distance),
				ranges::min(t_closed, compare, total_distance)},
				compare, total_distance);

			// Construct path from trees
			auto path = s_tree.path_from_root_to(rendezvous);
			Vert v = rendezvous;
			for (Edge e; (e = t_tree.out_edge_or_null(v)) != this->null_edge(); v = this->head(e))
				path.push_back(e);
			return path;
		}
	}
}
