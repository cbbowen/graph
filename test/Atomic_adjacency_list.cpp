
#include <graph/Atomic_adjacency_list.hpp>

#include "Graph_tester.hpp"

#include <range/v3/distance.hpp>

SCENARIO("atomic out-adjacency lists behave properly", "[Atomic_out_adjacency_list]") {
	using G = graph::Atomic_out_adjacency_list;
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
		#ifdef _OPENMP
		std::vector<std::mt19937> thread_random;
		for (int i = 0; i < omp_get_max_threads(); ++i)
			thread_random.emplace_back(i);
		const std::size_t M = 1000, N = 10000;
		WHEN("vertices are inserted in parallel") {
			#pragma omp parallel for
			for (int i = 0; i < M; ++i)
				g.insert_vert();
			REQUIRE(g.order() == M);
			REQUIRE(g.size() == 0);
			REQUIRE(ranges::distance(g.verts()) == g.order());
			REQUIRE(ranges::distance(g.edges()) == g.size());
		}
		WHEN("self-edges are inserted in parallel") {
			g.insert_vert();
			#pragma omp parallel for
			for (int i = 0; i < N; ++i) {
				auto u = g.random_vert(thread_random[omp_get_thread_num()]),
					v = g.random_vert(thread_random[omp_get_thread_num()]);
				g.insert_edge(u, v);
			}
			REQUIRE(g.order() == 1);
			REQUIRE(g.size() == N);
			REQUIRE(ranges::distance(g.verts()) == g.order());
			REQUIRE(ranges::distance(g.edges()) == g.size());
		}
		WHEN("edges and vertices are inserted in parallel") {
			#pragma omp parallel for
			for (int i = 0; i < M; ++i) {
				g.insert_vert();
				for (int j = 0; j < (N + i) / M; ++j) {
					auto u = g.random_vert(thread_random[omp_get_thread_num()]),
						v = g.random_vert(thread_random[omp_get_thread_num()]);
					g.insert_edge(u, v);
				}
			}
			REQUIRE(g.order() == M);
			REQUIRE(g.size() == N);
			REQUIRE(ranges::distance(g.verts()) == g.order());
			REQUIRE(ranges::distance(g.edges()) == g.size());
		}
		#endif
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
	}
}

SCENARIO("atomic in-adjacency lists behave properly", "[Atomic_in_adjacency_list]") {
	using G = graph::Atomic_in_adjacency_list;
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
		#ifdef _OPENMP
		std::vector<std::mt19937> thread_random;
		for (int i = 0; i < omp_get_max_threads(); ++i)
			thread_random.emplace_back(i);
		const std::size_t M = 1000, N = 10000;
		WHEN("vertices are inserted in parallel") {
			#pragma omp parallel for
			for (int i = 0; i < M; ++i)
				g.insert_vert();
			REQUIRE(g.order() == M);
			REQUIRE(g.size() == 0);
			REQUIRE(ranges::distance(g.verts()) == g.order());
			REQUIRE(ranges::distance(g.edges()) == g.size());
		}
		WHEN("edges are inserted in parallel") {
			for (int i = 0; i < M; ++i)
				g.insert_vert();
			#pragma omp parallel for
			for (int i = 0; i < N; ++i) {
				auto u = g.random_vert(thread_random[omp_get_thread_num()]),
					v = g.random_vert(thread_random[omp_get_thread_num()]);
				g.insert_edge(u, v);
			}
			REQUIRE(g.order() == M);
			REQUIRE(g.size() == N);
			REQUIRE(ranges::distance(g.verts()) == g.order());
			REQUIRE(ranges::distance(g.edges()) == g.size());
		}
		WHEN("vertices and edges are inserted in parallel") {
			#pragma omp parallel for
			for (int i = 0; i < M; ++i) {
				g.insert_vert();
				for (int j = 0; j < (N + i) / M; ++j) {
					auto u = g.random_vert(thread_random[omp_get_thread_num()]),
						v = g.random_vert(thread_random[omp_get_thread_num()]);
					g.insert_edge(u, v);
				}
			}
			REQUIRE(g.order() == M);
			REQUIRE(g.size() == N);
			REQUIRE(ranges::distance(g.verts()) == g.order());
			REQUIRE(ranges::distance(g.edges()) == g.size());
		}
		#endif
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

#ifdef GRAPH_BENCHMARK
TEST_CASE("atomic adjacency list", "[benchmark]") {
	using G = graph::Atomic_out_adjacency_list;
	static const std::size_t order = 1000;
	static const std::size_t size = 1000;
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
		auto [tree, distances] = g.shortest_paths_from(g.random_vert(r), weight);
	}
#	ifdef _OPENMP
	std::vector<std::mt19937> thread_random;
	for (int i = 0; i < omp_get_max_threads(); ++i)
		thread_random.emplace_back(i);
	BENCHMARK("insert vertices in parallel") {
		G g;
		#pragma omp parallel for
		for (int i = 0; i < order; ++i)
			g.insert_vert();
		REQUIRE(g.order() == order);
	}
	BENCHMARK("insert self-edges in parallel") {
		G g;
		g.insert_vert();
		#pragma omp parallel for
		for (int i = 0; i < size; ++i) {
			auto u = g.random_vert(thread_random[omp_get_thread_num()]),
				v = g.random_vert(thread_random[omp_get_thread_num()]);
			g.insert_edge(u, v);
		}
		REQUIRE(g.size() == size);
	}
	BENCHMARK("insert vertices and edges in parallel") {
		G g;
		#pragma omp parallel for
		for (int i = 0; i < order; ++i) {
			g.insert_vert();
			for (int j = 0; j < (size + i) / order; ++j) {
				auto u = g.random_vert(thread_random[omp_get_thread_num()]),
					v = g.random_vert(thread_random[omp_get_thread_num()]);
				g.insert_edge(u, v);
			}
		}
		REQUIRE(g.order() == order);
		REQUIRE(g.size() == size);
	}
#	endif
}
#endif
