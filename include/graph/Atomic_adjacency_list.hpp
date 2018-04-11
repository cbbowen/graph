#pragma once

#include "Graph.hpp"
#include "impl/Atomic_adjacency_list.hpp"

namespace graph {
	inline namespace v1 {
		using Atomic_out_adjacency_list = Out_edge_graph<
			impl::Atomic_out_adjacency_list>;

		using Atomic_in_adjacency_list = In_edge_graph<
			impl::Atomic_in_adjacency_list>;
	}
}
