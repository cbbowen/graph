
#include <graph/Adjacency_list.hpp>

#include "Graph_tester.hpp"

SCENARIO("out-adjacency lists behave properly", "[Out_adjacency_list]") {
	using G = graph::Out_adjacency_list;
	GIVEN("an empty graph") {
		G g;
		REQUIRE(g.order() == 0);
		REQUIRE(g.size() == 0);
		Out_edge_graph_tester gt{g};

		WHEN("a vertex is inserted and erased") {
			auto v = gt.insert_vert();
			gt.erase_vert(v);
		}
		WHEN("a self-edge is inserted and erased") {
			auto v = gt.insert_vert();
			auto e = gt.insert_edge(v, v);
			gt.erase_edge(e);
			gt.erase_vert(v);
		}
		WHEN("an edge is inserted and erased") {
			auto s = gt.insert_vert(),
				t = gt.insert_vert();
			auto e = gt.insert_edge(s, t);
			gt.erase_edge(e);
			gt.erase_vert(s);
			gt.erase_vert(t);
		}
		WHEN("a self-edge is inserted and its tail is erased") {
			auto v = gt.insert_vert();
			auto e = gt.insert_edge(v, v);
			gt.erase_vert(v);
			REQUIRE(g.order() == 0);
			REQUIRE(g.size() == 0);
		}
		WHEN("an edge is inserted and its tail is erased") {
			auto s = gt.insert_vert(),
				t = gt.insert_vert();
			auto e = gt.insert_edge(s, t);
			gt.erase_vert(s);
			REQUIRE(g.order() == 1);
			REQUIRE(g.size() == 0);
		}
		WHEN("the graph is cleared") {
			gt.clear();
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

		WHEN("a vertex is inserted and erased") {
			auto v = gt.insert_vert();
			gt.erase_vert(v);
		}
		WHEN("a self-edge is inserted and erased") {
			auto v = gt.insert_vert();
			auto e = gt.insert_edge(v, v);
			gt.erase_edge(e);
			gt.erase_vert(v);
		}
		WHEN("an edge is inserted and erased") {
			auto s = gt.insert_vert(),
				t = gt.insert_vert();
			auto e = gt.insert_edge(s, t);
			gt.erase_edge(e);
			gt.erase_vert(s);
			gt.erase_vert(t);
		}
		WHEN("all the edges are erased") {
			while (g.size())
				gt.erase_edge(gt.random_edge(r));
		}
		WHEN("viewed in reverse") {
			auto rg = g.reverse_view();
			In_edge_graph_tester rgt{rg};
		}
		WHEN("searching for shortest paths from a vertex") {
			auto s = gt.random_vert(r);
			auto weight = [](auto v) { return 1; };
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
}

SCENARIO("in-adjacency lists behave properly", "[In_adjacency_list]") {
	using G = graph::In_adjacency_list;
	GIVEN("an empty graph") {
		G g;
		REQUIRE(g.order() == 0);
		REQUIRE(g.size() == 0);
		In_edge_graph_tester gt{g};

		WHEN("a vertex is inserted and erased") {
			auto v = gt.insert_vert();
			gt.erase_vert(v);
		}
		WHEN("a self-edge is inserted and erased") {
			auto v = gt.insert_vert();
			auto e = gt.insert_edge(v, v);
			gt.erase_edge(e);
			gt.erase_vert(v);
		}
		WHEN("an edge is inserted and erased") {
			auto s = gt.insert_vert(),
				t = gt.insert_vert();
			auto e = gt.insert_edge(s, t);
			gt.erase_edge(e);
			gt.erase_vert(s);
			gt.erase_vert(t);
		}
		WHEN("a self-edge is inserted and its head is erased") {
			auto v = gt.insert_vert();
			auto e = gt.insert_edge(v, v);
			gt.erase_vert(v);
			REQUIRE(g.order() == 0);
			REQUIRE(g.size() == 0);
		}
		WHEN("an edge is inserted and its head is erased") {
			auto s = gt.insert_vert(),
				t = gt.insert_vert();
			auto e = gt.insert_edge(s, t);
			gt.erase_vert(t);
			REQUIRE(g.order() == 1);
			REQUIRE(g.size() == 0);
		}
		WHEN("the graph is cleared") {
			gt.clear();
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

		WHEN("a vertex is inserted and erased") {
			auto v = gt.insert_vert();
			gt.erase_vert(v);
		}
		WHEN("a self-edge is inserted and erased") {
			auto v = gt.insert_vert();
			auto e = gt.insert_edge(v, v);
			gt.erase_edge(e);
			gt.erase_vert(v);
		}
		WHEN("an edge is inserted and erased") {
			auto s = gt.insert_vert(),
				t = gt.insert_vert();
			auto e = gt.insert_edge(s, t);
			gt.erase_edge(e);
			gt.erase_vert(s);
			gt.erase_vert(t);
		}
		WHEN("all the edges are erased") {
			while (g.size())
				gt.erase_edge(gt.random_edge(r));
		}
		WHEN("the graph is cleared") {
			gt.clear();
		}
		WHEN("viewed in reverse") {
			auto rg = g.reverse_view();
			Out_edge_graph_tester rgt{rg};
		}
		WHEN("searching for shortest paths to a vertex") {
			auto t = gt.random_vert(r);
			auto weight = [](auto v) { return 1; };
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

TEST_CASE("adjacency list", "[benchmark]") {
	using G = graph::Out_adjacency_list;
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
	BENCHMARK("query adjacencies") {
		G g;
		for (std::size_t i = 0; i < order; ++i)
			g.insert_vert();
		for (std::size_t i = 0; i < size; ++i) {
			auto u = g.random_vert(r), v = g.random_vert(r);
			g.insert_edge(u, v);
		}
		std::size_t total_degrees = 0;
		for (auto v : g.verts())
			for (auto e : g.out_edges(v))
				++total_degrees;
		REQUIRE(g.order() == order);
		REQUIRE(g.size() == size);
		REQUIRE(total_degrees == size);
	}
	BENCHMARK("erase vertices") {
		G g;
		for (std::size_t i = 0; i < order; ++i)
			g.insert_vert();
		for (std::size_t i = 0; i < order; ++i)
			g.erase_vert(g.random_vert(r));
		REQUIRE(g.order() == 0);
	}
	BENCHMARK("erase edges") {
		G g;
		for (std::size_t i = 0; i < order; ++i)
			g.insert_vert();
		for (std::size_t i = 0; i < size; ++i) {
			auto u = g.random_vert(r), v = g.random_vert(r);
			g.insert_edge(u, v);
		}
		for (std::size_t i = 0; i < size; ++i)
			g.erase_edge(g.random_edge(r));
		REQUIRE(g.size() == 0);
	}
}
