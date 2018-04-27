#pragma once

#include "Graph.hpp"
#include "impl/Contiguous_atomic_adjacency_list.hpp"

namespace graph {
	inline namespace v1 {
		using Contiguous_atomic_out_adjacency_list = Out_edge_graph<
			impl::Contiguous_atomic_out_adjacency_list>;

		using Contiguous_atomic_in_adjacency_list = In_edge_graph<
			impl::Contiguous_atomic_in_adjacency_list>;
	}
}
