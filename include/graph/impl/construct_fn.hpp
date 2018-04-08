#pragma once

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class T>
			struct construct_fn {
				template <class... Args>
				constexpr T operator()(Args&&... args) const {
					return T(std::forward<Args>(args)...);
				}
			};
			template <class T>
			constexpr construct_fn<T> construct{};
		}
	}
}
