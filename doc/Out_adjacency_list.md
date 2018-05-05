
# Out_adjacency_list

Declared in `<graph/Adjacency_list.hpp>`:
```c++
class Out_adjacency_list;
```

Satisfies the [`Out_edge_graph`](Out_edge_graph.md) concept using a classical adjacency list data structure that supports removals.

## Member functions

In addition to the members required by the [`Out_edge_graph`](Out_edge_graph.md) concept, `Out_adjacency_list` provides functions to facilitate mutation.

| Member functions | | |
|------------------|-|-|
| `insert_vert()` | `Vert` | constructs a new vertex |
| `insert_edge(Vert s, Vert t)` | `Edge` | constructs a new edge with tail `s` and head `t` |
| `erase_vert(Vert v)` | | removes a vertex `v` with no incoming edges from other vertices |
| `erase_edge(Edge e)` | | removes edge `e` |
| `clear()` | | removes all vertices and edges |
