#pragma once

#include "impl/omp.hpp"
#ifdef _OPENMP

#include <limits>
#include <functional>
#include <algorithm>
#include <queue>
#include <vector>
#include <cassert>
#include <atomic>

#include <range/v3/algorithm/min.hpp>

#include "impl/exceptions.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			// This is identical to _bidirectional_search_step except that `near` and `far` closed sets are accessed atomically (and so must be maps to flags rather than sets)
			template <class Adjacency, class G, class Queue,
				class Near, class Far,
				class Weight, class Distance, class Tree,
				class Compare, class Combine>
			bool _atomic_bidirectional_search_step(const G& g, Queue& queue,
				Near& near, const Far& far,
				const Weight& weight, Distance& distance, Tree& tree,
				const Compare& compare, const Combine& combine) {
				auto [d, v] = queue.top();
				queue.pop();
				// this read doesn't need to be atomic because only this thread writes to `near`
				if (!near(v)) {

					// `std::atomic_store(&near[v], true, std::memory_order_relaxed);`
					auto& near_v = near[v];
					#pragma omp atomic write
					near_v = true;

					// auto done = `std::atomic_load(&far(v), std::memory_order_relaxed);`
					const auto& far_v = far(v);
					bool done;
					#pragma omp atomic read
					done = far_v;

					if (done)
						return true;
					for (auto e : traits::Adjacent_edges<Adjacency, G>::range(g, v)) {
						auto u = traits::adjacency_cokey<Adjacency, G>(g, e);
#ifdef NDEBUG
						// optimization not required for correctness but skips checks below
						if (near(u))
							continue;
#endif
						auto c = combine(d, weight(e));
#ifndef NDEBUG
						if (compare(c, d))
							throw precondition_unmet("negative weight edge");
#endif
						decltype(auto) du = distance[u];
						if (compare(c, du)) {
							assert(!near(u)); // sanity check which should never fail
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
		auto Bi_edge_graph<Impl>::parallel_shortest_path(const Vert& s, const Vert& t, const Weight& weight,
			const Compare& compare, const Combine& combine) const -> Path {
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
			using queue_type = std::priority_queue<pair_type, std::vector<pair_type>, queue_compare>;

			// Explores from one side
			auto explore = [&](auto adjacency, Vert v, auto& near, const auto& far, auto& distance, auto& tree, const std::atomic<bool>& stop, std::exception_ptr& ex) {
				try {
					auto queue = queue_type(queue_compare{ compare });
					queue.emplace(distance(v), v);
					while (!stop.load(std::memory_order_relaxed) && !queue.empty() &&
						!impl::_atomic_bidirectional_search_step<decltype(adjacency)>(this->_impl(),
							queue, near, far, weight, distance, tree, compare, combine))
						;
				} catch (...) {
					ex = std::current_exception();
				}
			};

			// Construct all the structures for each exploration
			auto s_closed = this->template ephemeral_vert_map<bool>(false),
				t_closed = this->template ephemeral_vert_map<bool>(false);
			auto s_distance = this->ephemeral_vert_map(inf),
				t_distance = this->ephemeral_vert_map(inf);
			s_distance[s] = zero;
			t_distance[t] = zero;
			auto s_tree = this->in_subtree(s);
			auto t_tree = this->out_subtree(t);
			alignas(64) std::atomic<bool> t_done{false}, s_done{false};
			std::exception_ptr s_ex{}, t_ex{};

			// Explore from both sides in parallel
			#pragma omp parallel num_threads(2)
			{
				#pragma omp single nowait
				{
					explore(impl::traits::Out{}, s, s_closed, t_closed, s_distance, s_tree, t_done, s_ex);
					s_done.store(true);
				}
				#pragma omp single nowait
				{
					explore(impl::traits::In{}, t, t_closed, s_closed, t_distance, t_tree, s_done, t_ex);
					t_done.store(true);
				}
			}
			// python or
			auto ex = std::min({std::move(s_ex), std::move(s_ex)},
				[](auto l, auto r){ return static_cast<bool>(l); });
			if (ex)
				std::rethrow_exception(ex);

			// Find the minimal rendezvous
			auto total_distance = [&](auto v) { return combine(s_distance(v), t_distance(v)); };
			auto rendezvous = ranges::min(this->verts(), compare, total_distance);
			if (!compare(total_distance(rendezvous), inf))
				return this->null_path();

			// Construct path from trees
			return this->concatenate_paths(
				s_tree.path_from_root_to(rendezvous),
				t_tree.path_to_root_from(rendezvous));
		}
	}
}

#else
namespace graph {
	inline namespace v1 {
		template <class Impl>
		template <class Weight, class Compare, class Combine>
		auto Bi_edge_graph<Impl>::parallel_shortest_path(const Vert& s, const Vert& t, const Weight& weight,
			const Compare& compare, const Combine& combine) const
			-> std::optional<std::vector<Edge>> {
			return this->shortest_path(s, t, weight, compare, combine);
		}
	}
}
#endif // _OPENMP
