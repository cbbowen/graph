
#include <graph/Stable_edge_list.hpp>

#include "Graph_tester.hpp"

SCENARIO("stable edge lists behave properly", "[Stable_edge_list]") {
	using G = graph::Stable_edge_list;
	GIVEN("an empty graph") {
		G g;
		REQUIRE(g.order() == 0);
		REQUIRE(g.size() == 0);
		Graph_tester gt{g};

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
		Graph_tester gt{g};

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
			Graph_tester rgt{rg};
		}
	}
}

#ifdef GRAPH_BENCHMARK
TEST_CASE("stable edge list", "[benchmark]") {
	using G = graph::Stable_edge_list;
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
}
#endif
