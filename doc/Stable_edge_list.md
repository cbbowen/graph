
# Stable_edge_list

Declared in `<graph/Stable_edge_list.hpp>`:
```c++
class Stable_edge_list;
```

Satisfies the [`Graph`](Graph.md) concept using a classical edge list data structure.

## Member functions

In addition to the members required by the [`Graph`](Graph.md) concept, `Stable_edge_list` provides functions to facilitate mutation.

| Member functions | | |
|------------------|-|-|
| `insert_vert()` | `Vert` | constructs a new vertex |
| `insert_edge(Vert s, Vert t)` | `Edge` | constructs a new edge with tail `s` and head `t` |
