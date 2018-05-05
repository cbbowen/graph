
# Mutable_map

```c++
template <class K, class T>
concept Mutable_map<K, T>;
```

Extends the [`FunctionObject`](http://en.cppreference.com/w/cpp/concept/FunctionObject)`<K> ⟶ T` concept with mutability.

This is similar to but weaker than a full [`AssociativeContainer`](http://en.cppreference.com/w/cpp/concept/AssociativeContainer).  In particular, it does not support iteration or erasure.  It is intended for representing arbitrary attributes of vertices and edges, like colors or weights.

## Template parameters
| Template parameters | |
|---------------------|-|
| `K` | key type |
| `T` | value type |

## Member functions
| Member functions | | |
|------------------|-|-|
| `operator()(K k) const` | `const T&` | returns the value associated with `k` |
| `operator[](K k)` | `T&` | returns a mutable reference to the value associated with `k` |
| `assign(K k, T t)` | | ** associates `t` with `k` |
| `exchange(K k, T t)` | `T` | ** exchanges `t` with the value associated with `k` |

\** _Experimental API that is likely to change._
