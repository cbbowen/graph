
#include <catch.hpp>

#include <graph/Stable_edge_list.hpp>

#include <tuple>
#include <set>
#include <sstream>

using namespace graph;
using namespace graph::attributes;

TEST_CASE("canonical dot format input", "[Dot_format]") {
	Stable_edge_list g;
	auto s = "digraph g { "
		"0 ; 1 ; 2 ; 3 ; 4 ; "
		"0 -> 1 ; 1 -> 2 ; 2 -> 0 ; 3 -> 4 ; 4 -> 3 ; "
	"}";
	std::istringstream(s) >> g.dot_format();
	REQUIRE(g.order() == 5);
	REQUIRE(g.size() == 5);
}

TEST_CASE("dot format input without explicit vertices", "[Dot_format]") {
	Stable_edge_list g;
	auto s = "digraph g { "
		"0 -> 1 ; 1 -> 2 ; 2 -> 0 ; 3 -> 4 ; 4 -> 3 ; "
	"}";
	std::istringstream(s) >> g.dot_format();
	REQUIRE(g.order() == 5);
	REQUIRE(g.size() == 5);
}

TEST_CASE("dot format input with edge chains", "[Dot_format]") {
	Stable_edge_list g;
	auto s = "digraph g { "
		"0 ; 1 ; 2 ; 3 ; 4 ; "
		"0 -> 1 -> 2 -> 0 ; 3 -> 4 -> 3 ; "
	"}";
	std::istringstream(s) >> g.dot_format();
	REQUIRE(g.order() == 5);
	REQUIRE(g.size() == 5);
}

TEST_CASE("dot format input without whitespace", "[Dot_format]") {
	Stable_edge_list g;
	auto s = "digraph g{"
		"0;1;2;3;4;"
		"0->1;1->2;2->0;3->4;4->3;"
	"}";
	std::istringstream(s) >> g.dot_format();
	REQUIRE(g.order() == 5);
	REQUIRE(g.size() == 5);
}

TEST_CASE("dot format input with mixed whitespace", "[Dot_format]") {
	Stable_edge_list g;
	auto s = "digraph\ng{\t"
		"0; 1;2\t;3;\n4\r;"
		"0->1; 1->2;2\t->0;3->\n4;4->3\r;"
	"} ";
	std::istringstream(s) >> g.dot_format();
	REQUIRE(g.order() == 5);
	REQUIRE(g.size() == 5);
}

TEST_CASE("canonical dot format input with integer vertex attributes", "[Dot_format]") {
	Stable_edge_list g;
	auto m = g.vert_map<int>();
	auto s = "digraph g { "
		"0 [ m = 0 ] ; 1 [ m = 1 ] ; 2 [ m = 2 ] ; 3 [ m = 3 ] ; 4 [ m = 4 ] ; "
		"0 -> 1 ; 1 -> 2 ; 2 -> 0 ; 3 -> 4 ; 4 -> 3 ; "
	"}";
	std::istringstream(s) >> g.dot_format("m"_of_vert = m);
	REQUIRE(g.order() == 5);
	REQUIRE(g.size() == 5);

	std::set<int> ms;
	for (auto v : g.verts())
		ms.insert(m(v));
	REQUIRE(ms.size() == g.order());
	for (int i = 0; i < g.order(); ++i)
		REQUIRE(ms.find(i) != ms.end());
}

TEST_CASE("dot format input with integer vertex attributes without whitespace", "[Dot_format]") {
	Stable_edge_list g;
	auto m = g.vert_map<int>();
	auto s = "digraph g{"
		"0[m=0];1[m=1];2[m=2];3[m=3];4[m=4];"
		"0->1;1->2;2->0;3->4;4->3;"
	"}";
	std::istringstream(s) >> g.dot_format("m"_of_vert = m);
	REQUIRE(g.order() == 5);
	REQUIRE(g.size() == 5);

	std::set<int> ms;
	for (auto v : g.verts())
		ms.insert(m(v));
	REQUIRE(ms.size() == g.order());
	for (int i = 0; i < g.order(); ++i)
		REQUIRE(ms.find(i) != ms.end());
}

TEST_CASE("canonical dot format input with string vertex attributes", "[Dot_format]") {
	Stable_edge_list g;
	auto m = g.vert_map<std::string>();
	auto s = R"(digraph g {
		0 [ m = "0" ] ; 1 [ m = "1" ] ; 2 [ m = "2" ] ; 3 [ m = "3" ] ; 4 [ m = "4" ] ; 
		0 -> 1 ; 1 -> 2 ; 2 -> 0 ; 3 -> 4 ; 4 -> 3 ; 
	})";
	std::istringstream(s) >> g.dot_format("m"_of_vert = m);
	REQUIRE(g.order() == 5);
	REQUIRE(g.size() == 5);

	std::set<std::string> ms;
	for (auto v : g.verts())
		ms.insert(m(v));
	REQUIRE(ms.size() == g.order());
	for (int i = 0; i < g.order(); ++i)
		REQUIRE(ms.find(std::to_string(i)) != ms.end());
}

TEST_CASE("dot format input with string vertex attributes without whitespace", "[Dot_format]") {
	Stable_edge_list g;
	auto m = g.vert_map<std::string>();
	auto s = "digraph g{"
		R"(0[m="0"];1[m="1"];2[m="2"];3[m="3"];4[m="4"];)"
		"0->1;1->2;2->0;3->4;4->3;"
	"}";
	std::istringstream(s) >> g.dot_format("m"_of_vert = m);
	REQUIRE(g.order() == 5);
	REQUIRE(g.size() == 5);

	std::set<std::string> ms;
	for (auto v : g.verts())
		ms.insert(m(v));
	REQUIRE(ms.size() == g.order());
	for (int i = 0; i < g.order(); ++i)
		REQUIRE(ms.find(std::to_string(i)) != ms.end());
}

TEST_CASE("canonical dot format input with integer edge attributes", "[Dot_format]") {
	Stable_edge_list g;
	auto m = g.edge_map<int>();
	auto s = "digraph g { "
		"0 ; 1 ; 2 ; 3 ; 4 ; "
		"0 -> 1 [ m = 0 ] ; 1 -> 2 [ m = 1 ] ; 2 -> 0 [ m = 2 ] ; 3 -> 4 [ m = 3 ] ; 4 -> 3 [ m = 4 ] ; "
	"}";
	std::istringstream(s) >> g.dot_format("m"_of_edge = m);
	REQUIRE(g.order() == 5);
	REQUIRE(g.size() == 5);

	std::set<int> ms;
	for (auto e : g.edges())
		ms.insert(m(e));
	REQUIRE(ms.size() == g.size());
	for (int i = 0; i < g.size(); ++i)
		REQUIRE(ms.find(i) != ms.end());
}

TEST_CASE("dot format input with integer edge attributes without whitespace", "[Dot_format]") {
	Stable_edge_list g;
	auto m = g.edge_map<int>();
	auto s = "digraph g{"
		"0;1;2;3;4;"
		"0->1[m=0];1->2[m=1];2->0[m=2];3->4[m=3];4->3[m=4];"
	"}";
	std::istringstream(s) >> g.dot_format("m"_of_edge = m);
	REQUIRE(g.order() == 5);
	REQUIRE(g.size() == 5);

	std::set<int> ms;
	for (auto e : g.edges())
		ms.insert(m(e));
	REQUIRE(ms.size() == g.size());
	for (int i = 0; i < g.size(); ++i)
		REQUIRE(ms.find(i) != ms.end());
}

TEST_CASE("canonical dot format input with string edge attributes", "[Dot_format]") {
	Stable_edge_list g;
	auto m = g.edge_map<std::string>();
	auto s = R"(digraph g {
		0 ; 1 ; 2 ; 3 ; 4 ; 
		0 -> 1 [ m = "0" ] ; 1 -> 2 [ m = "1" ] ; 2 -> 0 [ m = "2" ] ; 3 -> 4 [ m = "3" ] ; 4 -> 3 [ m = "4" ] ; 
	})";
	std::istringstream(s) >> g.dot_format("m"_of_edge = m);
	REQUIRE(g.order() == 5);
	REQUIRE(g.size() == 5);

	std::set<std::string> ms;
	for (auto e : g.edges())
		ms.insert(m(e));
	REQUIRE(ms.size() == g.size());
	for (int i = 0; i < g.size(); ++i)
		REQUIRE(ms.find(std::to_string(i)) != ms.end());
}

TEST_CASE("dot format input with string edge attributes without whitespace", "[Dot_format]") {
	Stable_edge_list g;
	auto m = g.edge_map<std::string>();
	auto s = "digraph g{"
		"0;1;2;3;4;"
		R"(0->1[m="0"];1->2[m="1"];2->0[m="2"];3->4[m="3"];4->3[m="4"];)"
	"}";
	std::istringstream(s) >> g.dot_format("m"_of_edge = m);
	REQUIRE(g.order() == 5);
	REQUIRE(g.size() == 5);

	std::set<std::string> ms;
	for (auto e : g.edges())
		ms.insert(m(e));
	REQUIRE(ms.size() == g.size());
	for (int i = 0; i < g.size(); ++i)
		REQUIRE(ms.find(std::to_string(i)) != ms.end());
}
