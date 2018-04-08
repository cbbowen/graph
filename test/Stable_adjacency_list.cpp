
#include <graph/Stable_adjacency_list.hpp>

#include "Graph_tester.hpp"

SCENARIO("stable out-adjacency lists behave properly", "[Stable_out_adjacency_list]") {
	using G = graph::Stable_out_adjacency_list;
	GIVEN("an empty graph") {
		G g;
		REQUIRE(g.order() == 0);
		REQUIRE(g.size() == 0);
		Out_edge_graph_tester gt{g};

		WHEN("a vertex is inserted") {
			gt.insert_vert();
		}
		WHEN("a self-edge is inserted") {
			auto v = gt.insert_vert();
			gt.insert_edge(v, v);
		}
		WHEN("an edge is inserted") {
			auto s = gt.insert_vert(),
				t = gt.insert_vert();
			g.insert_edge(s, t);
		}
	}
	GIVEN("a random graph") {
		std::mt19937 r;
		G g;
		const std::size_t M = 20, N = 100;
		for (std::size_t m = 0; m < M; ++m)
			g.insert_vert();
		for (std::size_t n = 0; n < N; ++n) {
			auto s = g.random_vert(r), t = g.random_vert(r);
			g.insert_edge(s, t);
		}
		REQUIRE(g.order() == M);
		REQUIRE(g.size() == N);
		Out_edge_graph_tester gt{g};

		WHEN("a vertex is inserted") {
			gt.insert_vert();
		}
		WHEN("a self-edge is inserted") {
			auto v = gt.insert_vert();
			gt.insert_edge(v, v);
		}
		WHEN("an edge is inserted") {
			auto s = gt.insert_vert(),
				t = gt.insert_vert();
			gt.insert_edge(s, t);
		}
		WHEN("viewed in reverse") {
			auto rg = g.reverse_view();
			In_edge_graph_tester rgt{rg};
		}
		WHEN("searching for shortest paths from a vertex") {
			auto weight = [](auto v) { return 1; };
			auto s = gt.random_vert(r);
			auto paths = g.shortest_paths_from(s, weight);
			REQUIRE(paths(s).first == 0);
			for (auto v : g.verts()) {
				auto e = paths(v).second;
				if (e != g.null_edge()) {
					REQUIRE(g.head(e) == v);
					REQUIRE(paths(v).first == paths(g.tail(e)).first + weight(e));
				}
			}
			for (auto e : g.edges()) {
				REQUIRE(!(paths(g.head(e)).first > paths(g.tail(e)).first + weight(e)));
			}
		}
	}
	//GIVEN("a complete out-adjacency list") {
	//	std::mt19937 r;
	//	G g;
	//	const std::size_t M = 100;
	//	for (std::size_t m = 0; m < M; ++m)
	//		g.insert_vert();
	//	for (auto s : g.verts()) {
	//		for (auto t : g.verts()) {
	//			g.insert_edge(s, t);
	//		}
	//	}
	//	REQUIRE(g.order() == M);
	//	REQUIRE(g.size() == M * M);
	//	Out_edge_graph_tester gt{g};

	//	WHEN("searching for weighted shortest paths from a vertex") {
	//		auto weight = g.edge_map(1.0);
	//		std::uniform_real_distribution<double> weight_dist;
	//		for (auto e : g.edges())
	//			weight[e] = weight_dist(r);
	//		auto s = gt.random_vert(r);
	//		auto paths = g.shortest_paths_from(s, weight);
	//		REQUIRE(paths(s).first == 0);
	//		for (auto v : g.verts()) {
	//			auto e = paths(v).second;
	//			if (e != g.null_edge()) {
	//				REQUIRE(g.head(e) == v);
	//				REQUIRE(paths(v).first == paths(g.tail(e)).first + weight(e));
	//			}
	//		}
	//		for (auto e : g.edges()) {
	//			REQUIRE(!(paths(g.head(e)).first > paths(g.tail(e)).first + weight(e)));
	//		}
	//	}
	//}
}

SCENARIO("stable in-adjacency lists behave properly", "[Stable_in_adjacency_list]") {
	using G = graph::Stable_in_adjacency_list;
	GIVEN("an empty graph") {
		G g;
		REQUIRE(g.order() == 0);
		REQUIRE(g.size() == 0);
		In_edge_graph_tester gt{g};

		WHEN("a vertex is inserted") {
			gt.insert_vert();
		}
		WHEN("a self-edge is inserted") {
			auto v = gt.insert_vert();
			gt.insert_edge(v, v);
		}
		WHEN("an edge is inserted") {
			auto s = gt.insert_vert(),
				t = gt.insert_vert();
			g.insert_edge(s, t);
		}
	}
	GIVEN("a random graph") {
		std::mt19937 r;
		G g;
		const std::size_t M = 20, N = 100;
		for (std::size_t m = 0; m < M; ++m)
			g.insert_vert();
		for (std::size_t n = 0; n < N; ++n) {
			auto s = g.random_vert(r), t = g.random_vert(r);
			g.insert_edge(s, t);
		}
		REQUIRE(g.order() == M);
		REQUIRE(g.size() == N);
		In_edge_graph_tester gt{g};

		WHEN("a vertex is inserted") {
			gt.insert_vert();
		}
		WHEN("a self-edge is inserted") {
			auto v = gt.insert_vert();
			gt.insert_edge(v, v);
		}
		WHEN("an edge is inserted") {
			auto s = gt.insert_vert(),
				t = gt.insert_vert();
			gt.insert_edge(s, t);
		}
		WHEN("viewed in reverse") {
			auto rg = g.reverse_view();
			Out_edge_graph_tester rgt{rg};
		}
		WHEN("searching for shortest paths to a vertex") {
			auto weight = [](auto v) { return 1; };
			auto t = gt.random_vert(r);
			auto paths = g.shortest_paths_to(t, weight);
			REQUIRE(paths(t).first == 0);
			for (auto v : g.verts()) {
				auto e = paths(v).second;
				if (e != g.null_edge()) {
					REQUIRE(g.tail(e) == v);
					REQUIRE(paths(v).first == paths(g.head(e)).first + weight(e));
				}
			}
			for (auto e : g.edges()) {
				REQUIRE(!(paths(g.tail(e)).first > paths(g.head(e)).first + weight(e)));
			}
		}
	}
}
