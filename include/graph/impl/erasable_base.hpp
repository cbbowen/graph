#pragma once

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class K>
			struct erasable_base {
				using key_type = K;
				virtual ~erasable_base() = default;
				virtual void _erase(const key_type&) = 0;
				virtual void _clear() = 0;
			};
		}
	}
}
