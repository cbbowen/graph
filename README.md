
[![codecov](https://codecov.io/gh/cbbowen/graph/branch/master/graph/badge.svg)](https://codecov.io/gh/cbbowen/graph)

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

| Data Structure              | Removals | Query Outgoing | Query Incoming |
| ---------------------------:|:--------:|:--------------:|:--------------:|
| `Out_adjacency_list`        | ✓        | ✓              | -              |
| `In_adjacency_list`         | ✓        | -              | ✓              |
| `Stable_out_adjacency_list` | -        | ✓              | -              |
| `Stable_in_adjacency_list`  | -        | -              | ✓              |
| `Edge_list`                 | ✓        | -              | -              |
| `Stable_edge_list`          | -        | -              | -              |

| Concept                  | Member                                          | Semantics
|:------------------------ |:----------------------------------------------- |:----------
| `Graph`                  | `Vert : EqualityComparable, LessThanComparable, Hashable` | Vertex
|                          | `verts() const ⟶ Range<Vert>`                  | Range over all vertices
|                          | `null_vert() const ⟶ Vert`                     | Unique vertex which will never comparable equal to one in the graph
|                          | `order() const ⟶ unsigned`                     | `size(verts())`
|                          | `vert_map<T>(T d = {}) ⟶ Map<Vert, T>`         | New map from vertices to `d`
|                          | `vert_set() ⟶ Set<Vert>`                       | New empty set of vertices
|                          | `Edge : EqualityComparable, LessThanComparable, Hashable`  | Edge
|                          | `edges() const ⟶ Range<Edge>`                  | Range over all edges
|                          | `null_edge() const ⟶ Edge`                     | Unique edge which will never comparable equal to one in the graph
|                          | `tail(Edge e) const ⟶ Vert`                    | Tail (or source) of `e`
|                          | `head(Edge e) const ⟶ Vert`                    | Head (or target) of `e`
|                          | `size() const ⟶ unsigned`                      | `size(edges())`
|                          | `edge_map<T>(T d = {}) ⟶ Map<Edge, T>`         | New map from edges to `d`
|                          | `edge_set() ⟶ Set<Edge>`                       | New empty set of edges
|                          | * `ephemeral_vert_map<T>(T d = {}) ⟶ Map<Vert, T>`| New map from vertices to `d` — changing the graph during the lifetime of an ephemeral object is undefined beahavior
|                          | * `ephemeral_vert_set() ⟶ Set<Vert>`              | New empty set of vertices — changing the graph during the lifetime of an ephemeral object is undefined beahavior
|                          | * `ephemeral_edge_map<T>(T d = {}) ⟶ Map<Edge, T>`| New map from edges to `d` — changing the graph during the lifetime of an ephemeral object is undefined beahavior
|                          | * `ephemeral_edge_set() ⟶ Set<Edge>`              | New empty set of edges — changing the graph during the lifetime of an ephemeral object is undefined beahavior
| `Out_edge_graph : Graph` | `out_edges(Vert) const ⟶ Range<Edge>`          | Range over all edges with a given tail
|                          | `out_degree(Vert v) const ⟶ unsigned`          | `size(out_edges(v))`
| `In_edge_graph : Graph`| `in_edges(Vert) const ⟶ Range<Edge>`  | Range over all edges with a given head
|                          | `in_degree(Vert v) const ⟶ unsigned`           | `size(in_edges(v))`
| `Set<K> : Range<K>`| `contains(K k) const ⟶ bool`              | Check if set contains `k`
|                          | `size() const ⟶ unsigned`                      | Size of set
|                          | `insert(K k) ⟶ bool`                           | Insert `k` if it is not in the set, and return indicating if it was inserted
|                          | ** `erase(K k) ⟶ bool`                         | Remove `k` if it is in the set, and return indicating if it was removed
|                          | ** `clear()`                                    | Make the set empty
| `Map<K,T>`               | `operator()(K k) const ⟶ const T&`             | Read-only value associated with `k`
|                          | `operator[](K k) ⟶ T&`                         | Writable value associated with `k`
|                          | ** `assign(K k, T t)`                           | Associate `t` with `k`
|                          | ** `exchange(K k, T t) ⟶ T`                    | Associate `t` with `k` and return the old value

\* Advanced API that should be avoided except when performance is important or in generic code.

\** Experimental API that may change without notice.

| Algorithm                  | Usage                                                    | Semantics
| --------------------------:|:-------------------------------------------------------- |:----------
| Dijkstra's                 | `out_graph.shortest_paths_from(v, weights, ...)`         | Finds the paths with minimum total edge `weights` from `v` to all other reachable vertices
|                            | `in_graph.shortest_paths_to(v, weights, ...)`            | Finds the paths with minimum total edge `weights` to `v` from all verticies which can reach it
| Prim's                     | `out_graph.minimum_tree_reachable_from(v, weights, ...)` | Finds the spanning tree with minimum total edge `weights` to all vertices reachable from `v`
|                            | `in_graph.minimum_tree_reaching_to(v, weights, ...)`     | Finds the spanning tree with minimum total edge `weights` from all vertices from which `v` is reachable
