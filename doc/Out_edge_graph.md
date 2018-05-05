
# Out_edge_graph

```c++
concept Out_edge_graph : Graph;
```

Extends the [`Graph`](Graph.md) concept with fast outgoing edge queries.

In this library, every data structure that satisfies this concept will be an instance of `Out_edge_graph<...>`, so to require that a function argument satisfies it, you need only use a signature like the following:

```c++
template <class G>
void some_function(const Out_edge_graph<G>& g);
```

## Member types
| Member type | Definition
|:----------- |:-----------
| `Out_degree` | Unsigned integer type

## Member functions
| Member functions | | |
|------------------|-|-|
| `out_edges(Vert s) const` | `Range<Edge>` | returns all edges with tail `s` |
| `out_degree(Vert s) const` | `Out_degree` | returns `size(out_edges(s))` |

| Algorithms | | |
|------------|-|-|
| `shortest_paths_from<W>(Vert s, Map<Edge, W> w)` | `pair<In_subtree, Map<Vert, Distance>>>` | finds the paths from `s` with minimum total edge weights `w` to all vertices |
| `minimum_tree_reachable_from<W>(Vert s, Map<Edge, W> w)` | `In_subtree` | finds the tree with minimum total edge weights `w` that spans vertices reachable from `v` |
