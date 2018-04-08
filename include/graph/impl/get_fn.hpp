#pragma once

#include <utility>

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <std::size_t I>
			struct get_fn {
				template <class Arg>
				constexpr decltype(auto) operator()(Arg&& arg) const {
					using ::std::get;
					return get<I>(std::forward<Arg>(arg));
				}
			};
			template <unsigned int I>
			constexpr get_fn<I> get{};
		}
	}
}
