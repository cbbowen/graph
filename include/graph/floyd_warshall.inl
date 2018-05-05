#pragma once

#include <limits>
#include <functional>
#include <cassert>

#include "impl/exceptions.hpp"
#include "impl/Subforest.hpp"

namespace graph {
	inline namespace v1 {
		template <class Impl>
		template <class Weight, class Compare, class Combine>
		auto Graph<Impl>::all_pairs_shortest_paths(const Weight& weight,
			const Compare& compare, const Combine& combine) const {
			// TODO: Convert these to parameters
			using D = std::decay_t<std::result_of_t<const Weight&(Edge)>>;
			auto zero = D{}, inf = std::numeric_limits<D>::infinity();

			// Build distance map
			auto distance = vert_map(vert_map(inf));
			for (auto v : verts())
				distance[v][v] = zero;
			for (auto e : edges()) {
				decltype(auto) d = distance[tail(e)][head(e)];
				d = std::min({d, weight(e)}, compare);
			}
			for (auto v : verts()) {
				decltype(auto) dv_ = distance(v);
				for (auto s : verts()) {
					decltype(auto) ds_ = distance[s];
					for (auto t : verts()) {
						decltype(auto) d = ds_[t];
						d = std::min({d, combine(ds_(v), dv_(t))}, compare);
					}
				}
			}

			return distance;

			//// Build trees from distance map
			//auto trees = vert_map(null_in_subtree());
			//for (auto s : verts()) {
			//	decltype(auto) ds_ = distance(s);
			//	auto tree = in_subtree(s);
			//	for (auto e_prime : edges()) {
			//		auto v = tail(e_prime), t = head(e_prime);
			//		auto d_prime = combine(ds_(v), weight(e_prime));
			//		auto e = tree.in_edge_or_null(t);
			//		if (is_null(e) || compare(d_prime,
			//			combine(ds_(tail(e)), weight(e))))
			//			tree.insert_edge(e_prime);
			//	}
			//	trees.assign(s, std::move(tree));
			//}
			// return std::pair(std::move(trees), std::move(distance));
		}
	}
}
