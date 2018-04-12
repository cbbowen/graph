
#include <iostream>
#include <random>

#include <graph/Adjacency_list.hpp>

using namespace std;
using namespace graph;
using namespace graph::attributes;

int main(int argc, char *argv[]) {
	// Build a random graph
	mt19937_64 random;
	Out_adjacency_list g;
	for (int i = 0; i < 8; ++i)
		g.insert_vert();
	for (int i = 0; i < 32; ++i) {
		auto u = g.random_vert(random), v = g.random_vert(random);
		g.insert_edge(u, v);
	}

	// Assign edge weights
	auto weight = g.edge_map<double>();
	for (auto e : g.edges())
		weight[e] = uniform_real_distribution(1.0, 2.0)(random);

	// Run Dijkstra's algorithm
	auto [tree, distance] = g.shortest_paths_from(g.random_vert(random), weight);

	// Output in dot format
	std::cout << tree.dot_format("distance"_of_vert = distance) << std::endl;
}
