#pragma once

#include "Graph.hpp"
#include "impl/Atomic_edge_list.hpp"

namespace graph {
	inline namespace v1 {
		using Atomic_edge_list = Graph<
			impl::Atomic_edge_list<>>;
	}
}
