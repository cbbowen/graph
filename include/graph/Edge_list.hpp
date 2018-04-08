#pragma once

#include "Graph.hpp"
#include "impl/Edge_list.hpp"

namespace graph {
	inline namespace v1 {
		// Classical edge list graph representation.
		using Edge_list = Graph<
			impl::Edge_list<>>;
	}
}
