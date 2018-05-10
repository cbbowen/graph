#pragma once

#include <utility>
#include <type_traits>

namespace graph {
	inline namespace v1 {
		namespace impl::traits {
			// TODO: These should defer to internal traits or adl to allow default implementations:
			//   range() => value_type (via ranges::range_value_type)
			//   range() => size() (via ranges::size)
			//   range() => size_type (via ranges::range_size_type)
			//   value_type => null() (via default constructor)
			//   value_type => map_type (via unordered_map)
			//   map_type => map(..) (via constructor)

			struct _unimplemented {};

			template <class Instance, class = void> struct _has_trait_helper :
				std::true_type {};
			template <class Instance> struct _has_trait_helper<Instance,
				std::enable_if_t<std::is_base_of_v<_unimplemented, Instance>>> :
				std::false_type {};
			template <template <class...> class Trait, class... Args>
			constexpr bool has_trait = _has_trait_helper<Trait<Args...>>{}();

			// Canonical place to read graph vertex traits.
			template <class G, class = void> struct Verts : _unimplemented {};
			template <class G> struct Verts<const G> : Verts<G> {};
			template <class G>
			struct Verts<G,
				std::void_t<
					typename G::Vert,
					typename G::Order>> {
				using value_type = typename G::Vert;
				using size_type = typename G::Order;
				static decltype(auto) range(const G& g) {
					return g.verts();
				}
				static size_type size(const G& g) {
					return g.order();
				}
				static value_type null(const G& g) {
					return g.null_vert();
				}

				using set_type = typename G::Vert_set;
				static set_type set(const G& g) {
					return g.vert_set();
				}
				using ephemeral_set_type = typename G::Ephemeral_vert_set;
				static ephemeral_set_type ephemeral_set(const G& g) {
					return g.ephemeral_vert_set();
				}

				template <class T>
				using map_type = typename G::template Vert_map<T>;
				template <class T>
				static map_type<T> map(const G& g, T default_) {
					return g.vert_map(std::move(default_));
				} // LCOV_EXCL_LINE (unreachable)
				template <class T>
				using ephemeral_map_type = typename G::template Ephemeral_vert_map<T>;
				template <class T>
				static ephemeral_map_type<T> ephemeral_map(const G& g, T default_) {
					return g.ephemeral_vert_map(std::move(default_));
				}
			};

			template <class G> constexpr bool has_verts = has_trait<Verts, G>;

			template <class G>
			struct Verts<G *, std::enable_if_t<has_verts<G>>> : Verts<G> {
				using Ptr = G *;
				using Base = Verts<G>;
				static decltype(auto) range(Ptr ptr) {
					return Base::range(*ptr);
				}
				static decltype(auto) size(Ptr ptr) {
					return Base::size(*ptr);
				}
				static decltype(auto) null(Ptr ptr) {
					return Base::null(*ptr);
				}
				static decltype(auto) set(Ptr ptr) {
					return Base::set(*ptr);
				}
				static decltype(auto) ephemeral_set(Ptr ptr) {
					return Base::ephemeral_set(*ptr);
				}
				template <class T>
				static decltype(auto) map(Ptr ptr, T default_) {
					return Base::map(*ptr, std::move(default_));
				} // LCOV_EXCL_LINE (unreachable)
				template <class T>
				static decltype(auto) ephemeral_map(Ptr ptr, T default_) {
					return Base::ephemeral_map(*ptr, std::move(default_));
				}
			};

			template <class G>
			struct Verts<std::reference_wrapper<G>, std::enable_if_t<has_verts<G>>> : Verts<G> {
				using Ref = std::reference_wrapper<G>;
				using Base = Verts<G>;
				static decltype(auto) range(Ref ref) {
					return Base::range(ref.get());
				}
				static decltype(auto) size(Ref ref) {
					return Base::size(ref.get());
				}
				static decltype(auto) null(Ref ref) {
					return Base::null(ref.get());
				}
				static decltype(auto) set(Ref ref) {
					return Base::set(ref.get());
				}
				static decltype(auto) ephemeral_set(Ref ref) {
					return Base::ephemeral_set(ref.get());
				}
				template <class T>
				static decltype(auto) map(Ref ref, T default_) {
					return Base::map(ref.get(), std::move(default_));
				} // LCOV_EXCL_LINE (unreachable)
				template <class T>
				static decltype(auto) ephemeral_map(Ref ref, T default_) {
					return Base::ephemeral_map(ref.get(), std::move(default_));
				}
			};

			// Canonical place to read graph edge traits.
			template <class G, class = void> struct Edges : _unimplemented {};
			template <class G> struct Edges<const G> : Edges<G> {};
			template <class G>
			struct Edges<G,
				std::void_t<
					typename G::Edge,
					typename G::Size>> {
				using value_type = typename G::Edge;
				using size_type = typename G::Size;
				static decltype(auto) range(const G& g) {
					return g.edges();
				}
				static size_type size(const G& g) {
					return g.size();
				}
				static value_type null(const G& g) {
					return g.null_edge();
				}
				static decltype(auto) tail(const G& g,
					const value_type& e) {
					return g.tail(e);
				}
				static decltype(auto) head(const G& g,
					const value_type& e) {
					return g.head(e);
				}

				using set_type = typename G::Edge_set;
				static set_type set(const G& g) {
					return g.edge_set();
				}
				using ephemeral_set_type = typename G::Ephemeral_edge_set;
				static ephemeral_set_type ephemeral_set(const G& g) {
					return g.ephemeral_edge_set();
				}

				template <class T>
				using map_type = typename G::template Edge_map<T>;
				template <class T>
				static map_type<T> map(const G& g, T default_) {
					return g.edge_map(std::move(default_));
				} // LCOV_EXCL_LINE (unreachable)
				template <class T>
				using ephemeral_map_type = typename G::template Ephemeral_edge_map<T>;
				template <class T>
				static ephemeral_map_type<T> ephemeral_map(const G& g, T default_) {
					return g.ephemeral_edge_map(std::move(default_));
				}
			};

			template <class G> constexpr bool has_edges = has_trait<Edges, G>;

			template <class G>
			struct Edges<G *, std::enable_if_t<has_edges<G>>> : Edges<G> {
				using Ptr = G *;
				using Base = Edges<G>;
				static decltype(auto) range(Ptr ptr) {
					return Base::range(*ptr);
				}
				static decltype(auto) size(Ptr ptr) {
					return Base::size(*ptr);
				}
				static decltype(auto) null(Ptr ptr) {
					return Base::null(*ptr);
				}
				template <class E>
				static decltype(auto) tail(Ptr ptr, E&& e) {
					return Base::tail(*ptr, std::forward<E>(e));
				}
				template <class E>
				static decltype(auto) head(Ptr ptr, E&& e) {
					return Base::head(*ptr, std::forward<E>(e));
				}
				static decltype(auto) set(Ptr ptr) {
					return Base::set(*ptr);
				}
				static decltype(auto) ephemeral_set(Ptr ptr) {
					return Base::ephemeral_set(*ptr);
				}
				template <class T>
				static decltype(auto) map(Ptr ptr, T default_) {
					return Base::map(*ptr, std::move(default_));
				} // LCOV_EXCL_LINE (unreachable)
				template <class T>
				static decltype(auto) ephemeral_map(Ptr ptr, T default_) {
					return Base::ephemeral_map(*ptr, std::move(default_));
				}
			};

			template <class G>
			struct Edges<std::reference_wrapper<G>, std::enable_if_t<has_edges<G>>> : Edges<G> {
				using Ref = std::reference_wrapper<G>;
				using Base = Edges<G>;
				static decltype(auto) range(Ref ref) {
					return Base::range(ref.get());
				}
				static decltype(auto) size(Ref ref) {
					return Base::size(ref.get());
				}
				static decltype(auto) null(Ref ref) {
					return Base::null(ref.get());
				}
				template <class E>
				static decltype(auto) tail(Ref ref, E&& e) {
					return Base::tail(ref.get(), std::forward<E>(e));
				}
				template <class E>
				static decltype(auto) head(Ref ref, E&& e) {
					return Base::head(ref.get(), std::forward<E>(e));
				}
				static decltype(auto) set(Ref ref) {
					return Base::set(ref.get());
				}
				static decltype(auto) ephemeral_set(Ref ref) {
					return Base::ephemeral_set(ref.get());
				}
				template <class T>
				static decltype(auto) map(Ref ref, T default_) {
					return Base::map(ref.get(), std::move(default_));
				} // LCOV_EXCL_LINE (unreachable)
				template <class T>
				static decltype(auto) ephemeral_map(Ref ref, T default_) {
					return Base::ephemeral_map(ref.get(), std::move(default_));
				}
			};

			// Canonical place to read graph outgoing edge traits.
			template <class G, class = void> struct Out_edges : _unimplemented {};
			template <class G> struct Out_edges<const G> : Out_edges<G> {};
			template <class G>
			struct Out_edges<G,
				std::void_t<
					typename Verts<G>::value_type,
					typename Edges<G>::value_type,
					typename G::Out_degree>> {
				using key_type = typename Verts<G>::value_type;
				using value_type = typename Edges<G>::value_type;
				using size_type = typename G::Out_degree;
				static decltype(auto) range(const G& g,
					const key_type& v) {
					return g.out_edges(v);
				}
				static size_type size(const G& g,
					const key_type& v) {
					return g.out_degree(v);
				}
			};

			template <class G> constexpr bool has_out_edges = has_trait<Out_edges, G>;

			template <class G>
			struct Out_edges<G *, std::enable_if_t<has_out_edges<G>>> : Out_edges<G> {
				using Ptr = G *;
				using Base = Out_edges<G>;
				static decltype(auto) range(Ptr ptr,
					const typename Base::key_type& v) {
					return Base::range(*ptr, v);
				}
				static decltype(auto) size(Ptr ptr,
					const typename Base::key_type& v) {
					return Base::size(*ptr, v);
				}
			};

			template <class G>
			struct Out_edges<std::reference_wrapper<G>, std::enable_if_t<has_out_edges<G>>> : Out_edges<G> {
				using Ref = std::reference_wrapper<G>;
				using Base = Out_edges<G>;
				static decltype(auto) range(Ref ref,
					const typename Base::key_type& v) {
					return Base::range(ref, v);
				}
				static decltype(auto) size(Ref ref,
					const typename Base::key_type& v) {
					return Base::size(ref, v);
				}
			};

			// Canonical place to read graph incoming edge traits.
			template <class G, class = void> struct In_edges : _unimplemented {};
			template <class G> struct In_edges<const G> : In_edges<G> {};
			template <class G>
			struct In_edges<G,
				std::void_t<
					typename Verts<G>::value_type,
					typename Edges<G>::value_type,
					typename G::In_degree>> {
				using key_type = typename Verts<G>::value_type;
				using value_type = typename Edges<G>::value_type;
				using size_type = typename G::In_degree;
				static decltype(auto) range(const G& g,
					const key_type& v) {
					return g.in_edges(v);
				}
				static size_type size(const G& g,
					const key_type& v) {
					return g.in_degree(v);
				}
			};

			template <class G> constexpr bool has_in_edges = has_trait<In_edges, G>;

			template <class G>
			struct In_edges<G *, std::enable_if_t<has_in_edges<G>>> : In_edges<G> {
				using Ptr = G *;
				using Base = In_edges<G>;
				static decltype(auto) range(Ptr ptr,
					const typename Base::key_type& v) {
					return Base::range(*ptr, v);
				}
				static decltype(auto) size(Ptr ptr,
					const typename Base::key_type& v) {
					return Base::size(*ptr, v);
				}
			};

			template <class G>
			struct In_edges<std::reference_wrapper<G>, std::enable_if_t<has_in_edges<G>>> : In_edges<G> {
				using Ref = std::reference_wrapper<G>;
				using Base = In_edges<G>;
				static decltype(auto) range(Ref ref,
					const typename Base::key_type& v) {
					return Base::range(ref, v);
				}
				static decltype(auto) size(Ref ref,
					const typename Base::key_type& v) {
					return Base::size(ref, v);
				}
			};

			template <class G>
			struct Insert_verts : Verts<G> {
				template <class... Args>
				static typename Verts<G>::value_type insert(G& g,
					Args&&... args) {
					return g.insert_vert(std::forward<Args>(args)...);
				}
			};

			template <class G>
			struct Insert_verts<std::reference_wrapper<G>> :
				Verts<std::reference_wrapper<G>> {
				using Ref = std::reference_wrapper<G>;
				template <class... Args>
				static typename Verts<G>::value_type insert(Ref ref,
					Args&&... args) {
					return Insert_verts<G>::insert(ref,
						std::forward<Args>(args)...);
				}
			};

			template <class G>
			struct Insert_edges : Edges<G> {
				template <class... Args>
				static typename Edges<G>::value_type insert(G& g,
					Args&&... args) {
					return g.insert_edge(std::forward<Args>(args)...);
				}
			};

			template <class G>
			struct Insert_edges<std::reference_wrapper<G>> :
				Edges<std::reference_wrapper<G>> {
				using Ref = std::reference_wrapper<G>;
				template <class... Args>
				static typename Edges<G>::value_type insert(Ref ref,
					Args&&... args) {
					return Insert_edges<G>::insert(ref,
						std::forward<Args>(args)...);
				}
			};

			template <class G> constexpr bool has_bi_edges = has_out_edges<G> && has_in_edges<G>;

			// Adjacency tags
			struct Out {};
			struct In {};

			template <class Adjacency>
			struct _adjacency_helper;
			template <> struct _adjacency_helper<Out> {
				using reverse_adjacency = In;
				template <class G> using adjacent_edges = Out_edges<G>;

				template <class G>
				static decltype(auto) key(const G& g, const typename Edges<G>::value_type& e) {
					return Edges<G>::tail(g, e);
				}
				template <class G>
				static decltype(auto) cokey(const G& g, const typename Edges<G>::value_type& e) {
					return Edges<G>::head(g, e);
				}
			};
			template <> struct _adjacency_helper<In> {
				using reverse_adjacency = Out;
				template <class G> using adjacent_edges = In_edges<G>;

				template <class G>
				static decltype(auto) key(const G& g, const typename Edges<G>::value_type& e) {
					return Edges<G>::head(g, e);
				}
				template <class G>
				static decltype(auto) cokey(const G& g, const typename Edges<G>::value_type& e) {
					return Edges<G>::tail(g, e);
				}
			};

			template <class Adjacency>
			using Reverse_adjacency = typename _adjacency_helper<Adjacency>::reverse_adjacency;

			template <class Adjacency, class G>
			using Adjacent_edges = typename _adjacency_helper<Adjacency>::template adjacent_edges<G>;

			template <class Adjacency, class G, class E>
			decltype(auto) adjacency_key(const G& g, E&& e) {
				return _adjacency_helper<Adjacency>::key(g, std::forward<E>(e));
			}

			template <class Adjacency, class G, class E>
			decltype(auto) adjacency_cokey(const G& g, E&& e) {
				return _adjacency_helper<Adjacency>::cokey(g, std::forward<E>(e));
			}
		}
	}
}
