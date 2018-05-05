
# Path

```c++
template <class Edge>
concept Path<Edge>;
```

Represents a path through a graph.

A path through a graph comprises a source vertex and a sequence of zero or more edges where the head of each vertex is the tail of the next.  Furthermore, the tail of the first edge, if it exists must be the source.  Similarly, if the last edge exists, we say the target of the path is its head.  Otherwise, the target is the same as the source, and we say the path is trivial.

## Template parameters
| Template parameters | |
|---------------------|-|
| `Edge` | edge type |

## Member functions
| Member functions | | |
|------------------|-|-|
| `edges() const` | `Range<Edge>` | returns the edges of the path from source to target |
| `is_trivial_or_null() const` | `bool` | checks if the path is trivial (or null) |
| `total<W>(Map<Edge, W> w)` | `W` | returns the total edge weight `w` |
