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
			template <class Range, class Random>
			auto sample_one(Range&& range, Random& random) {
				// It would be convenient if we could use this, but it contains an illegal goto that Clang refuses to compile
				//std::decay_t<ranges::range_value_type_t<Range>> a[1];
				//ranges::sample(std::forward<Range>(range), a, random);
				//return a[0];

				using size_type = ranges::range_size_type_t<Range>;
				if constexpr (ranges::RandomAccessRange<Range>()) {
					// Optimized case when range supports random access
					auto size = ranges::size(range);
					check_precondition(size > 0, "size must be positive");
					std::uniform_int_distribution<size_type> index_dist(0, size-1);
					return ranges::at(std::forward<Range>(range), index_dist(random));
				}
				
				std::decay_t<ranges::range_value_type_t<Range>> a;
				size_type size = 0;
				for (auto b : std::forward<Range>(range))
					if (!std::uniform_int_distribution<size_type>(0, size++)(random))
						a = b;
				check_precondition(size > 0, "size must be positive");
				return a;
			}
		}
		template <class Impl>
		template <class Random>
		auto Graph<Impl>::random_vert(Random& r) const -> Vert {
			return impl::sample_one(verts(), r);
		}
		template <class Impl>
		template <class Random>
		auto Graph<Impl>::random_edge(Random& r) const -> Edge {
			return impl::sample_one(edges(), r);
		}
	}
}
