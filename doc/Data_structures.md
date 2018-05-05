
# Data structures

Follow the hyperlinked names below for documentation on the individual data structures.

| Data Structure              | Insertion | Removal | Query Outgoing | Query Incoming |
|:--------------------------- |:---------:|:-------:|:--------------:|:--------------:|
| [`Edge_list`](Edge_list.md) | ✓         | ✓       |                |                |
| [`Out_adjacency_list`](Out_adjacency_list.md)        | ✓         | ✓       | ✓              |                |
| [`In_adjacency_list`](In_adjacency_list.md)         | ✓         | ✓       |                | ✓              |
| [`Bi_adjacency_list`](Bi_adjacency_list.md)         | ✓         | ✓       | ✓              | ✓              |
| [`Stable_edge_list`](Stable_edge_list.md)          | ✓         |         |                |                |
| [`Stable_out_adjacency_list`](Stable_out_adjacency_list.md) | ✓         |         | ✓              |                |
| [`Stable_in_adjacency_list`](Stable_in_adjacency_list.md)  | ✓         |         |                | ✓              |
| [`Stable_bi_adjacency_list`](Stable_bi_adjacency_list.md)  | ✓         |         | ✓              | ✓              |
| [`Atomic_edge_list`](Atomic_edge_list.md)          | _atomic_  |         |                |                |
| [`Atomic_out_adjacency_list`](Atomic_out_adjacency_list.md) | _atomic_  |         | _atomic_       |                |
| [`Atomic_in_adjacency_list`](Atomic_in_adjacency_list.md) | _atomic_  |         |                | _atomic_       |

Note that the data structures that do not support removal are generally prefixed with `Stable_` to indicate that their vertices and edges are never invalidated.  To enable application to parallel domains, lock-free `Atomic_` graphs are also available.
