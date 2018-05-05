
# Graph

```c++
concept Graph;
```

Represents a finite, directed graph.

A [directed graph](https://en.wikipedia.org/wiki/Directed_graph) comprises a set of vertices and a set of edges.  Each edge has both a head vertex and a tail vertex which may or may not be equal.

In this library, every data structure that satisfies this concept will be an instance of `Graph<...>`, so to require that a function argument satisfies it, you need only use a signature like the following:

```c++
template <class G>
void some_function(const Graph<G>& g);
```

## Member types
| Member type | Definition
|:----------- |:-----------
| `Vert`        | `EqualityComparable`, `LessThanComparable`, `Hashable`
| `Order`       | Unsigned integer type
| `Vert_set`    | [`Mutable_set`](Mutable_set.md)`<Vert>`
| `Vert_map<T>` | [`Mutable_map`](Mutable_map.md)`<Vert, T>`
| `Edge`        | `EqualityComparable`, `LessThanComparable`, `Hashable`
| `Size`        | Unsigned integer type
| `Edge_set`    | [`Mutable_set`](Mutable_set.md)`<Edge>`
| `Edge_map<T>` | [`Mutable_map`](Mutable_map.md)`<Edge, T>`
| `Path`        | [`Path`](Path.md)`<Edge>`
| `Out_subforest` | Subgraph with edges up to one or more roots
| `In_subforest` | Subgraph with edges down from one or more roots
| `Out_subtree` | Subgraph with edges up to a single root
| `In_subforest` | Subgraph with edges down from a single root
| `Ephemeral_vert_set` | * [`Mutable_set`](Mutable_set.md)`<Vert>` _(it is undefined behavior to modify a graph during the lifetime of an ephemeral type constructed from it)_
| `Ephemeral_vert_map<T>` | * [`Mutable_map`](Mutable_map.md)`<Vert, T>` _(it is undefined behavior to modify a graph during the lifetime of an ephemeral type constructed from it)_
| `Ephemeral_edge_set` | * [`Mutable_set`](Mutable_set.md)`<Edge>` _(it is undefined behavior to modify a graph during the lifetime of an ephemeral type constructed from it)_
| `Ephemeral_edge_map<T>` | * [`Mutable_map`](Mutable_map.md)`<Edge, T>` _(it is undefined behavior to modify a graph during the lifetime of an ephemeral type constructed from it)_

\* _Advanced API that should be avoided except in generic code or when performance is critical._

## Member functions
| Vertices | | |
|----------|-|-|
| `verts() const` | `Range<Vert>` | returns all vertices |
| `null_vert() const` | `Vert` | returns a `Vert` never equal to one in the graph |
| `is_null(Vert v) const` | `bool` | returns `v == null_vert()`
| `order() const` | `Order` | returns `size(verts())` |
| `vert_set() const` | `Vert_set` | constructs an empty, mutable set of vertices |
| `vert_map<T>(T d) const` | `Vert_map<T>` | constructs a mutable map from vertices to `d` |

| Edges | | |
|-------|-|-|
| `edges() const` | `Range<Edge>` | returns all edges |
| `null_edge() const` | `Edge` | returns an `Edge` never equal to one in the graph |
| `is_null(Edge e) const` | `bool` | returns `e == null_edge()`
| `size() const` | `Size` | returns `size(edges())` |
| `edge_set() const` | `Edge_set` | constructs an empty, mutable set of edges |
| `edge_map<T>(T d) const` | `Edge_map<T>` | constructs a mutable map from edges to `d` |
| `tail(Edge e) const` | `Vert` | source of `e` |
| `head(Edge e) const` | `Vert` | target of `e` |

| Paths | | |
|-------|-|-|
| `null_path() const` | `Path` | returns a `Path` never equal to one in the graph |
| `is_null(Path p) const` | `bool` | returns `p == null_path()`
| `is_trivial(Path p) const` | `bool` | checks if `p` is trivial (and non-null) |
| `source(Path p) const` | `Vert` | returns the source of `p` |
| `target(Path p) const` | `Vert` | returns the target of `p` |
| `path(Vert s, Range<Edges> es) const` | `Path` | returns a new path from its source and sequence of adjacent edges |
| `concatenate_paths(Path p0, Path p1, ...) const` | `Path` | returns the concatenation of two or more paths |

| Views | | |
|-------|-|-|
| `reverse_view() const` | `Graph` | returns view of the graph with all edges reversed |
| `dot_format(...)` | | returns a view of the graph that supports streaming to and from dot format |

| Random | | |
|--------|-|-|
| `random_vert(RNG&) const` | `Vert` | returns a vertex selected uniformly at random |
| `random_edge(RNG&) const` | `Edge` | returns an edge selected uniformly at random |

| * Ephemeral | | |
|-------------|-|-|
| `ephemeral_vert_set() const` | `Ephemeral_vert_set` | constructs an empty, mutable set of vertices |
| `ephemeral_vert_map<T>(T d) const` | `Ephemeral_vert_map<T>` | constructs a mutable map from vertices to `d` |
| `ephemeral_edge_set() const` | `Ephemeral_edge_set` | constructs an empty, mutable set of edges |
| `ephemeral_edge_map<T>(T d) const` | `Ephemeral_edge_map<T>` | constructs a mutable map from edges to `d` |

| * Subtrees | | |
|------------|-|-|
| `out_subforest() const` | `Out_subforest` | constructs an out-subforest with no edges |
| `in_subforest() const` | `In_subforest` | constructs a in-subforest with no edges |
| `out_subtree(Vert r) const` | `Out_subtree` | constructs an empty out-subtree rooted at `r` |
| `in_subtree(Vert r) const` | `In_subtree` | constructs an empty in-subtree rooted at `r` |

\* _Advanced API that should be avoided except in generic code or when performance is critical._
