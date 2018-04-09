#pragma once

#include <type_traits>
#include <random>
#include <range/v3/range_traits.hpp>

// See `graph::impl::sample_one` below
//#include <range/v3/algorithm/sample.hpp>
#include <range/v3/at.hpp>

#include "impl/exceptions.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class Range, class Size, class Random>
			auto sample_one(Range&& range, Size size, Random& random) {
#ifndef NDEBUG
				if (size <= 0)
					throw precondition_unmet("size must be positive");
#endif
				// It would be convenient if we could use this, but it contains an illegal goto that Clang refuses to compile
				//std::decay_t<ranges::range_value_type_t<Range>> a[1];
				//ranges::sample(std::forward<Range>(range), a, random);
				//return a[0];

				using size_type = ranges::range_size_type_t<Range>;
				if constexpr (ranges::RandomAccessRange<Range>()) {
					// Optimized case when range supports random access
					std::uniform_int_distribution<size_type> index_dist(0, size-1);
					return ranges::at(std::forward<Range>(range), index_dist(random));
				}

				std::decay_t<ranges::range_value_type_t<Range>> a;
				size_type n = 0;
				for (auto b : std::forward<Range>(range))
					if (!std::uniform_int_distribution<size_type>(0, n++)(random))
						a = b;
				return a;
			}
		}
		template <class Impl>
		template <class Random>
		auto Graph<Impl>::random_vert(Random& r) const -> Vert {
			return impl::sample_one(verts(), order(), r);
		}
		template <class Impl>
		template <class Random>
		auto Graph<Impl>::random_edge(Random& r) const -> Edge {
			return impl::sample_one(edges(), size(), r);
		}
	}
}
