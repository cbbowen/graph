#pragma once

#include "Graph.hpp"
#include "impl/Stable_adjacency_list.hpp"

namespace graph {
	inline namespace v1 {
		// Classical adjacency list graph representation with outgoing edge iteration which does not support removal.
		using Stable_out_adjacency_list = Out_edge_graph<
			impl::Stable_out_adjacency_list<>>;

		// Classical adjacency list graph representation with incoming edge iteration which does not support removal.
		using Stable_in_adjacency_list = In_edge_graph<
			impl::Stable_in_adjacency_list<>>;
	}
}
