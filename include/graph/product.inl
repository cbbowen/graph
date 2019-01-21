#pragma once

#include "impl/Tensor_product.hpp"

namespace graph {
	inline namespace v1 {
		template <class... G>
		auto tensor_product_view(const Graph<G>&... g) {
			return _wrap_graph(impl::Tensor_product(&g._impl()...));
		}
	}
}
