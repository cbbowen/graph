#include <chrono>
#include <random>
#include <vector>

// Boost.Graph
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>

// This library
// Note that Boost's adjacency list invalidates vertex and edge descriptors and indices on removals, so an apples-to-apples comparison that includes removals is more difficult.
#include <graph/Stable_adjacency_list.hpp>

// Number of vertices and edges to insert.
int M = 100, N = 1000;

template <class RNG>
void with_this_library(RNG& rng) {
	// Generate a random graph
	graph::Stable_out_adjacency_list g;
	for (int i = 0; i < M; ++i)
		g.insert_vert();
	for (int i = 0; i < N; ++i) {
		auto u = g.random_vert(rng), v = g.random_vert(rng);
		g.insert_edge(u, v);
	}
	// Build a map of degrees by iterating over edges
	auto out_degrees = g.vert_map(0);
	for (auto e : g.edges())
		++out_degrees[g.tail(e)];
	// Make sure the map agrees with the graph by iterating over vertices
	for (auto v : g.verts())
		if (out_degrees[v] != g.out_degree(v))
			throw std::runtime_error("degrees did not match");
}

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
	typename property_map<adjacency_list<>, vertex_index_t>::type vertex_index_map = get(vertex_index, g);
	std::vector<int> out_degrees(num_vertices(g));
	for (auto [it, end] = edges(g); it != end; ++it)
		++out_degrees[get(vertex_index_map, source(*it, g))];
	// Make sure the map agrees with the graph by iterating over vertices
	for (auto [it, end] = vertices(g); it != end; ++it)
		if (out_degrees[get(vertex_index_map, *it)] != out_degree(*it, g))
			throw std::runtime_error("degrees did not match");
}

int main(int argc, char *argv[]) {
	bool timing = false;
#ifdef NDEBUG
	timing = true;
#endif
	using clock = std::chrono::high_resolution_clock;
	{
		std::mt19937_64 random;
		auto start = clock::now();
		for (int i = 0; i < (timing ? 1000 : 1); ++i)
			with_this_library(random);
		if (timing)
			std::cout << "elapsed time (This Library): " << (clock::now() - start).count() << std::endl;
	}
	{
		std::mt19937_64 random;
		auto start = clock::now();
		for (int i = 0; i < (timing ? 1000 : 1); ++i)
			with_boost_graph(random);
		if (timing)
			std::cout << "elapsed time (Boost.Graph): " << (clock::now() - start).count() << std::endl;
	}
}
