
#include <graph/Stable_adjacency_list.hpp>

#include "Graph_tester.hpp"

#include <numeric> // for std::accumulate

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
			assert(g.size()); // sanity check on the test itself
			auto rg = g.reverse_view();
			In_edge_graph_tester rgt{rg};
		}
		WHEN("searching for shortest paths from a vertex") {
			auto s = gt.random_vert(r);
			//auto weight = [](auto e) { return 1; };
			auto weight = g.edge_map(0.0);
			for (auto e : g.edges())
				weight[e] = std::uniform_real_distribution<double>{}(r);
			auto [tree, distances] = g.shortest_paths_from(s, weight);
			REQUIRE(tree.root() == s);
			REQUIRE(distances(s) == 0);
			for (auto v : g.verts()) {
				auto e = tree.in_edge_or_null(v);
				if (e != g.null_edge()) {
					REQUIRE(g.head(e) == v);
					REQUIRE(distances(v) == distances(g.tail(e)) + weight(e));
				}
			}
			for (auto e : g.edges())
				REQUIRE(!(distances(g.head(e)) > distances(g.tail(e)) + weight(e)));
		}
		WHEN("searching for the shortest paths between all pairs of vertices") {
			auto weight = [](auto e) { return 1; };
			auto distances = g.all_pairs_shortest_paths(weight);
			//auto [trees, distances] = g.all_pairs_shortest_paths(weight);
			//// Verify that the distances and trees agree
			//for (auto s : g.verts()) {
			//	for (auto t : g.verts()) {
			//		auto path = trees(s).path_from_root_to(t);
			//		if (g.is_null(path))
			//			REQUIRE(distances(s)(t) >= g.order());
			//		else
			//			REQUIRE(distances(s)(t) == path.total(weight));
			//	}
			//}
			// Verify that the distances are accurate
			for (auto s : g.verts()) {
				auto [_, distance_s] = g.shortest_paths_from(s, weight);
				for (auto t : g.verts())
					REQUIRE(distances(s)(t) == distance_s(t));
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
			assert(g.size()); // sanity check on the test itself
			auto rg = g.reverse_view();
			Out_edge_graph_tester rgt{rg};
		}
		WHEN("searching for shortest paths to a vertex") {
			auto t = gt.random_vert(r);
			//auto weight = [](auto e) { return 1; };
			auto weight = g.edge_map(0.0);
			for (auto e : g.edges())
				weight[e] = std::uniform_real_distribution<double>{}(r);
			auto [tree, distances] = g.shortest_paths_to(t, weight);
			REQUIRE(tree.root() == t);
			REQUIRE(distances(t) == 0);
			for (auto v : g.verts()) {
				auto e = tree.out_edge_or_null(v);
				if (e != g.null_edge()) {
					REQUIRE(g.tail(e) == v);
					REQUIRE(distances(v) == distances(g.head(e)) + weight(e));
				}
			}
			for (auto e : g.edges())
				REQUIRE(!(distances(g.tail(e)) > distances(g.head(e)) + weight(e)));
		}
	}
}

SCENARIO("stable bi-adjacency lists behave properly", "[Stable_bi_adjacency_list]") {
	using G = graph::Stable_bi_adjacency_list;
	GIVEN("an empty graph") {
		G g;
		REQUIRE(g.order() == 0);
		REQUIRE(g.size() == 0);
		Bi_edge_graph_tester gt{g};

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
			assert(g.size()); // sanity check on the test itself
			auto rg = g.reverse_view();
			Bi_edge_graph_tester rgt{rg};
		}
		WHEN("searching for the shortest path between vertices") {
			auto weight = g.edge_map(0.0);
			const double epsilon = 0.001;
			for (auto e : g.edges())
				weight[e] = std::uniform_real_distribution(epsilon, 1.0)(r);
			// Insert an extra vertex that is never reachable
			gt.insert_vert();
			for (auto s : g.verts()) {
				auto [tree, distance] = g.shortest_paths_from(s, weight);
				for (auto t : g.verts()) {
					auto path = g.shortest_path(s, t, weight);
					if (s == t) {
						REQUIRE(g.is_trivial(path));
					} else if (!g.is_null(path)) {
						// Verify the path starts and end in the correct places
						REQUIRE(g.source(path) == s);
						REQUIRE(g.target(path) == t);
						// Verify this is the shortest path against Dijkstra's
						auto path_distance = path.total(weight);
						REQUIRE(path_distance <= distance(t) + epsilon * g.order());
					} else {
						// Verify no path exists
						REQUIRE(!tree.in_tree(t));
					}
				}
			}
		}
		WHEN("searching for the shortest path between vertices in parallel") {
			auto weight = g.edge_map(0.0);
			const double epsilon = 0.001;
			for (auto e : g.edges())
				weight[e] = std::uniform_real_distribution(epsilon, 1.0)(r);
			// Insert an extra vertex that is never reachable
			gt.insert_vert();
			for (auto s : g.verts()) {
				auto [tree, distance] = g.shortest_paths_from(s, weight);
				for (auto t : g.verts()) {
					auto path = g.parallel_shortest_path(s, t, weight);
					if (s == t) {
						REQUIRE(g.is_trivial(path));
					} else if (!g.is_null(path)) {
						// Verify the path starts and end in the correct places
						REQUIRE(g.source(path) == s);
						REQUIRE(g.target(path) == t);
						// Verify this is the shortest path against Dijkstra's
						auto path_distance = path.total(weight);
						REQUIRE(path_distance <= distance(t) + epsilon * g.order());
					} else {
						// Verify no path exists
						REQUIRE(!tree.in_tree(t));
					}
				}
			}
		}
	}
}

#ifdef GRAPH_BENCHMARK
TEST_CASE("stable adjacency list", "[benchmark]") {
	using G = graph::Stable_out_adjacency_list;
	static const std::size_t order = 1000;
	static const std::size_t size = 10000;
	std::mt19937 r;

	BENCHMARK("insert vertices") {
		G g;
		for (std::size_t i = 0; i < order; ++i)
			g.insert_vert();
		REQUIRE(g.order() == order);
	}
	BENCHMARK("insert self-edges") {
		G g;
		auto v = g.insert_vert();
		for (std::size_t i = 0; i < size; ++i)
			g.insert_edge(v, v);
		REQUIRE(g.size() == size);
	}
	BENCHMARK("insert random edges") {
		G g;
		for (std::size_t i = 0; i < order; ++i)
			g.insert_vert();
		for (std::size_t i = 0; i < size; ++i) {
			auto u = g.random_vert(r), v = g.random_vert(r);
			g.insert_edge(u, v);
		}
		REQUIRE(g.order() == order);
		REQUIRE(g.size() == size);
	}

	G g;
	for (std::size_t i = 0; i < order; ++i)
		g.insert_vert();
	for (std::size_t i = 0; i < size; ++i) {
		auto u = g.random_vert(r), v = g.random_vert(r);
		g.insert_edge(u, v);
	}

	BENCHMARK("query adjacencies") {
		std::size_t total_degrees = 0;
		for (auto v : g.verts())
			for (auto e : g.out_edges(v))
				++total_degrees;
		REQUIRE(total_degrees == size);
	}
	BENCHMARK("find single-source shortest paths") {
		auto weight = g.edge_map(0.0);
		for (auto e : g.edges())
			weight[e] = std::uniform_real_distribution<double>{}(r);
		for (auto s : g.verts()) {
			auto [_, distance] = g.shortest_paths_from(s, weight);
			REQUIRE(distance(s) == 0);
		}
	}
}

TEST_CASE("stable bidirectional adjacency list", "[benchmark]") {
	using G = graph::Stable_bi_adjacency_list;
	static const std::size_t order = 1000;
	static const std::size_t size = 10000;
	std::mt19937 r;

	BENCHMARK("insert vertices") {
		G g;
		for (std::size_t i = 0; i < order; ++i)
			g.insert_vert();
		REQUIRE(g.order() == order);
	}
	BENCHMARK("insert self-edges") {
		G g;
		auto v = g.insert_vert();
		for (std::size_t i = 0; i < size; ++i)
			g.insert_edge(v, v);
		REQUIRE(g.size() == size);
	}
	BENCHMARK("insert random edges") {
		G g;
		for (std::size_t i = 0; i < order; ++i)
			g.insert_vert();
		for (std::size_t i = 0; i < size; ++i) {
			auto u = g.random_vert(r), v = g.random_vert(r);
			g.insert_edge(u, v);
		}
		REQUIRE(g.order() == order);
		REQUIRE(g.size() == size);
	}

	G g;
	for (std::size_t i = 0; i < order; ++i)
		g.insert_vert();
	for (std::size_t i = 0; i < size; ++i) {
		auto u = g.random_vert(r), v = g.random_vert(r);
		g.insert_edge(u, v);
	}

	BENCHMARK("query adjacencies") {
		std::size_t total_degrees = 0;
		for (auto v : g.verts())
			for (auto e : g.out_edges(v))
				++total_degrees;
		REQUIRE(total_degrees == size);
	}
	BENCHMARK("find shortest path") {
		auto weight = g.edge_map(0.0);
		for (auto e : g.edges())
			weight[e] = std::uniform_real_distribution<double>{}(r);
		for (auto s : g.verts()) {
			auto path = g.shortest_path(s, g.random_vert(r), weight);
			if (!g.is_null(path))
				REQUIRE(path.total(weight) >= 0);
		}
	}
	BENCHMARK("find shortest path in parallel") {
		auto weight = g.edge_map(0.0);
		for (auto e : g.edges())
			weight[e] = std::uniform_real_distribution<double>{}(r);
		for (auto s : g.verts()) {
			auto path = g.parallel_shortest_path(s, g.random_vert(r), weight);
			if (!g.is_null(path))
				REQUIRE(path.total(weight) >= 0);
		}
	}
}
#endif
