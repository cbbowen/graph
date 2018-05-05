
# Out_edge_graph

```c++
concept Out_edge_graph : Graph;
```

Extends the [`Graph`](Graph.md) concept with fast outgoing edge queries.

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
