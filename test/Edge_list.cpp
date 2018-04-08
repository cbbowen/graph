
#include <graph/Edge_list.hpp>

#include "Graph_tester.hpp"

SCENARIO("edge sets behave properly", "[Edge_list]") {
	using G = graph::Edge_list;
	GIVEN("an empty graph") {
		G g;
		REQUIRE(g.order() == 0);
		REQUIRE(g.size() == 0);
		Graph_tester gt{g};

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
		Graph_tester gt{g};

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
			Graph_tester rgt{rg};
		}
	}
}
