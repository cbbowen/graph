#include <graph/Out_adjacency_list.hpp>
using namespace graph;

int main(int argc, char* argv[])
{
	// Build a random graph
	std::mt19937_64 random;
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
		weight[e] = std::uniform_real_distribution(1.0, 2.0)(random);

	// Run Dijkstra's algorithm
	auto [tree, distance] = g.shortest_paths_from(g.random_vert(random), weight);

	// Output result in dot format
	using namespace graph::attributes; // for `_of_vert`
	std::cout << tree.dot_format("distance"_of_vert = distance) << std::endl;

	return 0;
}
