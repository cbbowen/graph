#pragma once

#include <stdexcept>

#ifdef NDEBUG
#	define GRAPH_CHECK_PRECONDITIONS 0
#else
#	define GRAPH_CHECK_PRECONDITIONS 1
#endif

namespace graph {
	inline namespace v1 {
		class precondition_unmet : std::logic_error {
			using logic_error::logic_error;
		};
	}
}
