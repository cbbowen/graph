#pragma once

#include "Graph.hpp"
#include "impl/Adjacency_list.hpp"

namespace graph {
	inline namespace v1 {
		// Classical adjacency list graph representation with outgoing edge iteration.
		using Out_adjacency_list = Out_edge_graph<
			impl::Out_adjacency_list<>>;

		// Classical adjacency list graph representation with incoming edge iteration.
		using In_adjacency_list = In_edge_graph<
			impl::In_adjacency_list<>>;
	}
}
