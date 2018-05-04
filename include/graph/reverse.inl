#pragma once

#include "impl/Reverse.hpp"

namespace graph {
	inline namespace v1 {
		template <class Impl>
		auto Graph<Impl>::reverse_view() const {
			return _wrap_graph(impl::Reverse(std::cref(this->_impl())));
		}
	}
}
