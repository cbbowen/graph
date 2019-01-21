
#include <graph/Edge_list.hpp>

#include "Graph_tester.hpp"

#include <sstream>

SCENARIO("tensor products behave properly", "[Tensor_product]") {
	GIVEN("two random edge lists") {
		std::mt19937 r;
		graph::Edge_list g0, g1;
		for (const auto ref : { std::ref(g0), std::ref(g1) }) {
			auto& g = ref.get();
			const std::size_t M = 5, N = 20;
			for (std::size_t m = 0; m < M; ++m)
				g.insert_vert();
			for (std::size_t n = 0; n < N; ++n) {
				auto s = g.random_vert(r), t = g.random_vert(r);
				g.insert_edge(s, t);
			}
		}

		WHEN("taking the product of a single graph") {
			auto product = graph::tensor_product_view(g0);
			REQUIRE(product.order() == g0.order());
			REQUIRE(product.size() == g0.size());
			Graph_tester pgt{ product };
		}

		WHEN("taking the product of a graph with itself") {
			auto product = graph::tensor_product_view(g0, g0);
			REQUIRE(product.order() == g0.order() * g0.order());
			REQUIRE(product.size() == g0.size() * g0.size());
			Graph_tester pgt{ product };
		}

		WHEN("taking the product of two graphs") {
			auto product = graph::tensor_product_view(g0, g1);
			REQUIRE(product.order() == g0.order() * g1.order());
			REQUIRE(product.size() == g0.size() * g1.size());
			Graph_tester pgt{ product };
		}
	}
}
