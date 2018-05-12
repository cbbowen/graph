
# Graph Concepts

Follow the hyperlinked names below for documentation on the individual concepts.

| Concept                               | |
|---------------------------------------|-|
| [`Graph`](Graph.md)                   | Basic graph concept |
| [`Out_edge_graph`](Out_edge_graph.md) | Graph supporting outgoing edge queries |
| [`In_edge_graph`](In_edge_graph.md)   | Graph supporting incoming edge queries |
| [`Bi_edge_graph`](Bi_edge_graph.md)   | Graph supporting bidirectional edge queries |

# Data structures

Follow the hyperlinked names below for documentation on the individual data structures.

| Data Structure                                              | Implements       | Insertion | Removal |
|-------------------------------------------------------------|------------------|:---------:|:-------:|
| [`Edge_list`](Edge_list.md)                                 | `Graph`          | ✓         | ✓       |
| [`Out_adjacency_list`](Out_adjacency_list.md)               | `Out_edge_graph` | ✓         | ✓       |
| [`In_adjacency_list`](In_adjacency_list.md)                 | `In_edge_graph`  | ✓         | ✓       |
| [`Bi_adjacency_list`](Bi_adjacency_list.md)                 | `Bi_edge_graph`  | ✓         | ✓       |
| [`Stable_edge_list`](Stable_edge_list.md)                   | `Graph`          | ✓         |         |
| [`Stable_out_adjacency_list`](Stable_out_adjacency_list.md) | `Out_edge_graph` | ✓         |         |
| [`Stable_in_adjacency_list`](Stable_in_adjacency_list.md)   | `In_edge_graph`  | ✓         |         |                | ✓              |
| [`Stable_bi_adjacency_list`](Stable_bi_adjacency_list.md)   | `Bi_edge_graph`  | ✓         |         | ✓              | ✓              |
| [`Atomic_edge_list`](Atomic_edge_list.md)                   | `Graph`          | _atomic_  |         |                |                |
| [`Atomic_out_adjacency_list`](Atomic_out_adjacency_list.md) | `Out_edge_graph` | _atomic_  |         | _atomic_       |                |
| [`Atomic_in_adjacency_list`](Atomic_in_adjacency_list.md)   | `In_edge_graph`  | _atomic_  |         |                | _atomic_       |

Note that the data structures that do not support removal are generally prefixed with `Stable_` to indicate that their vertices and edges are never invalidated.  To enable application to parallel domains, lock-free `Atomic_` graphs are also available.
