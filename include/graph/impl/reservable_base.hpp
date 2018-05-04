#pragma once

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class K, class Size = std::size_t>
			struct reservable_base {
				using size_type = Size;
				virtual ~reservable_base() = default;
				virtual void _reserve(size_type capacity) = 0;
			};
		}
	}
}
