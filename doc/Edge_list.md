
# Edge_list

Declared in `<graph/Edge_list.hpp>`:
```c++
class Edge_list;
```

Satisfies the [`Graph`](Graph.md) concept using a classical edge list data structure that supports removals.

## Member functions

In addition to the members required by the [`Graph`](Graph.md) concept, `Edge_list` provides functions to facilitate mutation.

| Member functions | | |
|------------------|-|-|
| `insert_vert()` | `Vert` | constructs a new vertex |
| `insert_edge(Vert s, Vert t)` | `Edge` | constructs a new edge with tail `s` and head `t` |
| `erase_vert(Vert v)` | | removes a vertex `v` with no edges |
| `erase_edge(Edge e)` | | removes edge `e` |
| `clear()` | | removes all vertices and edges |
