
# Easy

Graphs are ubiquitous, so they should be easy to use, from construction to queries, and algorithms to serialization.  Consider the following snippet demonstrating each of these features.

```cpp
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
auto shortest_paths = g.shortest_paths_from(g.random_vert(random), weight);

// Output in dot format
std::cout << g.dot_format("weight"_of_edge = weight) << std::endl;
```

# Efficient

This library firmly embraces the philosophy that you only pay for what you use.  For example, it provides graphs that support removal and those that do not because this capability impacts the performance characteristics of the data structure.  Where possible, it employs cache-friendly, contiguous layouts.

# Generic

By using a trait-driven implementation, everything is kept header-only and generic.  If your use case requires a specialized data structure, you need only implement the appropriate traits and all the algorithms implemented immediately become available.

This doesn't mean you should usually need to do so, of course.  This library provides multiple data structures out of the box and more are on the way.

| Graphs                      | Removals | Query Outgoing | Query Incoming |
| ---------------------------:|:--------:|:--------------:|:--------------:|
| `Out_adjacency_list`        | ✓        | ✓              | -              |
| `In_adjacency_list`         | ✓        | -              | ✓              |
| `Stable_out_adjacency_list` | -        | ✓              | -              |
| `Stable_in_adjacency_list`  | -        | -              | ✓              |
| `Edge_list`                 | ✓        | -              | -              |
| `Stable_edge_list`          | -        | -              | -              |

A `Graph` has associated data types `Vert` and `Edge` representing vertices and edges respectively.  These are `EqualityComparable`, `LessThanComparable`, and `CopyAssignable`.  They also specialize `std::hash` and can be streamed to `std::basic_ostream<char>` for easy debugging.

Given a `Graph` `g`, `Range`s over its vertices and edges are provided by `g.verts()` and `g.edges()`.  `g.null_vert()` and `g.null_edge()` are guaranteed to produce unique values that will not compare equal a vertex or edge in the graph.

Given an `Edge` `e`, its tail (source) and head (target) are queried via `g.tail(e)` and `g.head(e)`.  Many graphs support insertion of vertices and edges through `g.insert_vert()` and `g.insert_edge(u,v)`.  Some of the graphs additionally support removal through `g.erase_vert(v)` and `g.erase_edge(e)`.  However, care must be taken as these may have preconditions.

The real power of the library comes from `Vert_map`s and `Edge_map`s constructed through `g.vert_map<T>()` and `g.edge_map<T>()`.  These allow efficient mapping of vertices and edges to abitrary values, like vertex colors or edge weights.  Generally, the algorithms provided by the library simply expect `Callable`s to query these kinds of properties, which these types are.  However, vertex and edge maps are also writable via `operator[]`.
(Users of Boost.Graph will likely already be familiar with this basic design, but it has been streamlined to avoid `boost::get`.)
