#pragma once

#include <type_traits>
#include <random>
#include <range/v3/range_traits.hpp>
#include <range/v3/algorithm/sample.hpp>

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class Range, class Random>
			auto sample_one(Range&& range, Random& random) {
				// TODO: This could be implemented more efficiently for both sized and unsized ranges
				std::decay_t<ranges::range_value_type_t<Range>> a[1];
				ranges::sample(std::forward<Range>(range), a, random);
				return a[0];
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
