#pragma once

#include <stdexcept>

namespace graph {
	inline namespace v1 {
		class precondition_unmet : std::logic_error {
			using logic_error::logic_error;
		};
	}
}
