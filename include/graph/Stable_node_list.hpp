#pragma once

#include "Graph.hpp"
#include "impl/Stable_node_list.hpp"

namespace graph {
	inline namespace v1 {
		// Classical node list graph representation with outgoing edge iteration which does not support removal.
		template <class VData = std::tuple<>, class EData = std::tuple<>>
		using Stable_out_node_list = Out_edge_graph<
			impl::Stable_out_node_list<VData, EData>>;

		// Classical node list graph representation with incoming edge iteration which does not support removal.
		template <class VData = std::tuple<>, class EData = std::tuple<>>
		using Stable_in_node_list = In_edge_graph<
			impl::Stable_in_node_list<VData, EData>>;
	}
}
