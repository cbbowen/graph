#include <chrono>
#include <random>
#include <vector>

// Boost.Graph
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>

// This library
// Note that Boost's adjacency list invalidates vertex and edge descriptors and indices on removals, so an apples-to-apples comparison that includes removals is more difficult.
#include <graph/Stable_adjacency_list.hpp>

// Number of vertices and edges to insert.
int M = 100, N = 1000;

// The functions `with_this_library` and `with_boost_graph` perform the same set of basic operations but the first is implemented with this graph library, while the second uses Boost.Graph.  I'll let you decide which is simpler.  Of course, Boost.Graph is much more complete and includes an impressive range of graph algorithms albeit with a now dated interface.  If you compile with NDEBUG defined, basic timing information will also be reported with this library performing comparably or slightly better than Boost.Graph.

template <class RNG>
void with_boost_graph(RNG& rng) {
	// Generate a random graph
	using namespace boost;
	adjacency_list<> g;
	for (int i = 0; i < M; ++i)
		add_vertex(g);
	for (int i = 0; i < N; ++i) {
		auto u = random_vertex(g, rng), v = random_vertex(g, rng);
		add_edge(u, v, g);
	}

	// Build a map of degrees by iterating over edges
	auto vertex_index_map = get(vertex_index, g);
	std::vector<int> out_degrees(num_vertices(g));
	for (auto [it, end] = edges(g); it != end; ++it)
		++out_degrees[get(vertex_index_map, source(*it, g))];
	// Make sure the map agrees with the graph by iterating over vertices
	for (auto [it, end] = vertices(g); it != end; ++it)
		if (out_degrees[get(vertex_index_map, *it)] != out_degree(*it, g))
			throw std::runtime_error("degrees did not match");

	// Run Dijkstra's algorithm
	using vertex_descriptor = typename graph_traits<decltype(g)>::vertex_descriptor;
	using edge_descriptor = typename graph_traits<decltype(g)>::edge_descriptor;
	auto weights = make_static_property_map<edge_descriptor>(1.0);
	std::vector<vertex_descriptor> tree(num_vertices(g));
	std::vector<double> distances(num_vertices(g));
	dijkstra_shortest_paths(g, random_vertex(g, rng), weight_map(weights).
		predecessor_map(make_iterator_property_map(tree.begin(), vertex_index_map)).
		distance_map(make_iterator_property_map(distances.begin(), vertex_index_map)));
}

template <class RNG>
void with_this_library(RNG& rng) {
	// Generate a random graph
	// Notable difference: uses methods rather than free functions
	graph::Stable_out_adjacency_list g;
	for (int i = 0; i < M; ++i)
		g.insert_vert();
	for (int i = 0; i < N; ++i) {
		auto u = g.random_vert(rng), v = g.random_vert(rng);
		g.insert_edge(u, v);
	}

	// Build a map of degrees by iterating over edges
	// Notable differences:
	// - graphs know how to make performant maps from vertices and edges
	// - range-based for loops work as expected
	// - maps supply operator[] rather than requring `get` and `put` functions
	auto out_degrees = g.vert_map(0);
	for (auto e : g.edges())
		++out_degrees[g.tail(e)];
	// Make sure the map agrees with the graph by iterating over vertices
	for (auto v : g.verts())
		if (out_degrees[v] != g.out_degree(v))
			throw std::runtime_error("degrees did not match");

	// Run Dijkstra's algorithm
	// Notable differences:
	// - This only required two simple lines of code
	// - The result includes a tree with semantics beyond a simple predecessor map
	auto weights = [](auto e) { return 1.0; };
	auto [tree, distances] = g.shortest_paths_from(g.random_vert(rng), weights);
}

int main(int argc, char *argv[]) {
	{
		std::mt19937_64 random;
		with_this_library(random);
		with_boost_graph(random);
	}
#ifdef NDEBUG
	using clock = std::chrono::high_resolution_clock;
	{
		std::mt19937_64 random;
		auto start = clock::now();
		for (int i = 0; i < 10000; ++i)
			with_this_library(random);
		std::cout << "elapsed time (This Library): " << (clock::now() - start).count() << std::endl;
	}
	{
		std::mt19937_64 random;
		auto start = clock::now();
		for (int i = 0; i < 10000; ++i)
			with_boost_graph(random);
		std::cout << "elapsed time (Boost.Graph): " << (clock::now() - start).count() << std::endl;
	}
#endif
}
