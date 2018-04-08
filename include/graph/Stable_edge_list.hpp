#pragma once

#include "Graph.hpp"
#include "impl/Stable_edge_list.hpp"

namespace graph {
	inline namespace v1 {
		// Classical edge list graph representation which does not support removal.
		using Stable_edge_list = Graph<
			impl::Stable_edge_list<>>;
	}
}
