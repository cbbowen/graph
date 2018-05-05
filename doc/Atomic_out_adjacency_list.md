
# Atomic_out_adjacency_list

Declared in `<graph/Atomic_adjacency_list.hpp>`:
```c++
class Atomic_out_adjacency_list;
```

Satisfies the [`Out_edge_graph`](Out_edge_graph.md) concept using a classical adjacency list data structure adapted for parallel domains.

Specifically, all `const` methods and those prefixed with `atomic_*` guarantee atomicity.

## Member functions

In addition to the members required by the [`Out_edge_graph`](Out_edge_graph.md) concept, `Atomic_out_adjacency_list` provides functions to facilitate mutation.

| Vertices | | |
|------------------|-|-|
| `insert_vert()` | `Vert` | constructs a new vertex |
| `reserve_verts(Order n)` | | ensures that vertices can be inserted atomically up to order `n` |
| `atomic_insert_vert()` | `Vert` | atomically constructs a new vertex |

| Edges | | |
|------------------|-|-|
| `insert_edge(Vert s, Vert t)` | `Edge` | constructs a new edge with tail `s` and head `t` |
| `reserve_edges(Size n)` | | ensures that edges can be inserted atomically up to size `n` |
| `atomic_insert_edge(Vert s, Vert t)` | `Edge` | atomically constructs a new edge with tail `s` and head `t` |
