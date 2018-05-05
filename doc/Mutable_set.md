
# Mutable_set

```c++
template <class T>
concept Mutable_set<T>;
```

This is similar to but weaker than a full [`Container`](http://en.cppreference.com/w/cpp/concept/Container).  It is intended for representing sets of vertices or edges, like visted vertices or cut edges.

## Template parameters
| Template parameters | |
|---------------------|-|
| `T` | element type |

## Member functions
| Member functions | | |
|------------------|-|-|
| `contains(T e) const` | `bool` | checks if `e` is an element |
| `size() const` | Unsigned integer type | returns the number of elements |
| `insert(T e)` | `bool` | insert an element |
| `erase(T e)` | `bool` | ** erase an element |
| `clear()` | | ** erase all elements |

\** _Experimental API that is likely to change._
