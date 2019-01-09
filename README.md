
[![codecov](https://codecov.io/gh/cbbowen/graph/branch/master/graph/badge.svg)](https://codecov.io/gh/cbbowen/graph)

# Easy

Graphs are ubiquitous, so they should be easy to use, from construction to queries, and algorithms to serialization.  Consider the following snippet demonstrating each of these features.

```cpp
#include <graph/Out_adjacency_list.hpp>
using namespace graph;

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
```

For a detailed walkthrough of this example, see the [tutorial](doc/Tutorial.md).

For more examples, take a look in the `example` directory.  To compile and run any example file `NAME.cpp`, simply `make NAME` in that directory.  If you're already familiar with the extensive Boost.Graph library, `example/bgl.cpp` should provide a nice transition.

For more even more detail, see the [documentation overview](doc/Overview.md).

# Efficient

This library firmly embraces the philosophy that you only pay for what you use.  For example, it provides graphs that support removal and those that do not, because this capability impacts the performance characteristics of the data structure.  Where possible, it employs cache-friendly, contiguous layouts.

# Generic

By using a trait-driven implementation, everything is kept header-only and generic.  If your use case requires a specialized data structure, you need only implement the appropriate traits and all the algorithms implemented immediately become available.

This doesn't mean you should usually need to do so, of course.  This library provides multiple data structures out of the box and more are on the way.  The [documentation overview](doc/Overview.md) includes a tables of concepts and the data structures that implement them.

To declare a function accepting a generic `Graph` all you need to write is:

```cpp
template <class G>
void some_function(const graph::Graph<G>& g);
```

This works similarly for the [`Out_edge_graph`](doc/Out_edge_graph.md), [`In_edge_graph`](doc/In_edge_graph.md), and [`Bi_edge_graph`](doc/Bi_edge_graph.md) concepts.

# Building the tests with conan
Building the tests is easy given you have installed [conan](https://conan.io) and [CMake](http://cmake.org):

- `mkdir build`
- `cd build/`
- `conan install ..`
- `cmake ..`
- `make`

The tests executable should be found in `bin`.

# Contributing

If you find a bug or there is a data structure or algorithm you think should be added, please create an issue.  Or if you're feeling ambitious, implement the changes yourself and send a pull request.  Additional examples are also welcome!
