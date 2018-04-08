#pragma once

#include "impl/Reverse.hpp"

namespace graph {
	inline namespace v1 {
		template <class Impl>
		auto Graph<Impl>::reverse_view() const {
			// qualified name to avoid injected `using Graph = Graph<Impl>`
			return v1::Graph(impl::Reverse(std::cref(this->_impl())));
		}
		template <class Impl>
		auto Out_edge_graph<Impl>::reverse_view() const {
			return In_edge_graph(impl::Reverse(std::cref(this->_impl())));
		}
		template <class Impl>
		auto In_edge_graph<Impl>::reverse_view() const {
			return Out_edge_graph(impl::Reverse(std::cref(this->_impl())));
		}
	}
}
