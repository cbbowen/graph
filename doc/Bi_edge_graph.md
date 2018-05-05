
# Bi_edge_graph

```c++
concept Bi_edge_graph : Out_edge_graph, In_edge_graph;
```

Extends both the [`Out_edge_graph`](Out_edge_graph.md) and [`In_edge_graph`](In_edge_graph.md) concepts to provide bidirectional adjacency information.

## Member functions
| Algorithms | | |
|------------|-|-|
| `shortest_path<W>(Vert s, Vert t, Map<Edge, W> w)` | `Path` | finds the path from `s` to `t` with minimum total edge weights `w` |
| `parallel_shortest_path<W>(Vert s, Vert t, Map<Edge, W> w)` | `Path` | ** finds the path from `s` to `t` with minimum total edge weights `w` in parallel |

\** _Experimental API that is likely to change._
