
# In_edge_graph

```c++
concept In_edge_graph : Graph;
```

Extends the [`Graph`](Graph.md) concept with fast incoming edge queries.

## Member types
| Member type | Definition
|:----------- |:-----------
| `In_degree` | Unsigned integer type

## Member functions
| Member functions | | |
|------------------|-|-|
| `in_edges(Vert t) const` | `Range<Edge>` | returns all edges with head `t` |
| `in_degree(Vert t) const` | `In_degree` | returns `size(in_edges(t))` |

| Algorithms | | |
|------------|-|-|
| `shortest_paths_to<W>(Vert t, Map<Edge, W> w)` | `pair<Out_subtree, Map<Vert, Distance>>>` | finds the paths to `t` with minimum total edge weights `w` from all vertices |
| `minimum_tree_reaching_to<W>(Vert s, Map<Edge, W> w)` | `Out_subtree` | finds the tree with minimum total edge weights `w` that spans vertices from which `v` is reachable |
