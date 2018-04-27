#pragma once

#include "impl/omp.hpp"
#ifdef _OPENMP

#include <limits>
#include <functional>
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

			auto s_closed = this->template ephemeral_vert_map<bool>(false),
				t_closed = this->template ephemeral_vert_map<bool>(false);
			auto s_distance = this->ephemeral_vert_map(inf),
				t_distance = this->ephemeral_vert_map(inf);
			auto s_tree = this->in_subtree(s);
			auto t_tree = this->out_subtree(t);

			s_distance[s] = zero;
			t_distance[t] = zero;

			// Perform bidirectional search steps in parallel on two threads
			alignas(64) std::atomic<bool> t_done{false}, s_done{false};
			std::exception_ptr s_ex{}, t_ex{};
			#pragma omp parallel num_threads(2)
			{
				#pragma omp single nowait
				{
					try {
						auto s_queue = queue_type(queue_compare{ compare });
						s_queue.emplace(zero, s);
						while (!t_done.load(std::memory_order_relaxed) && !s_queue.empty() &&
							!impl::_atomic_bidirectional_search_step<impl::traits::Out>(this->_impl(),
								s_queue, s_closed, t_closed, weight, s_distance, s_tree, compare, combine))
							;
					} catch (...) {
						s_ex = std::current_exception();
					}
					s_done.store(true);
				}
				#pragma omp single nowait
				{
					try {
						auto t_queue = queue_type(queue_compare{ compare });
						t_queue.emplace(zero, t);
						while (!s_done.load(std::memory_order_relaxed) && !t_queue.empty() &&
							!impl::_atomic_bidirectional_search_step<impl::traits::In>(this->_impl(),
								t_queue, t_closed, s_closed, weight, t_distance, t_tree, compare, combine))
							;
					} catch (...) {
						t_ex = std::current_exception();
					}
					t_done.store(true);
				}
			}
			if (s_ex)
				std::rethrow_exception(s_ex);
			if (t_ex)
				std::rethrow_exception(t_ex);

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
