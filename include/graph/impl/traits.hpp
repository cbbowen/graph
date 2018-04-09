#pragma once

#include <utility>

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

			// Canonical place to read graph vertex traits.
			template <class G, class = void>
			struct Verts {
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
				}
				template <class T>
				using ephemeral_map_type = typename G::template Ephemeral_vert_map<T>;
				template <class T>
				static ephemeral_map_type<T> ephemeral_map(const G& g, T default_) {
					return g.ephemeral_vert_map(std::move(default_));
				}
			};

			template <class G>
			struct Verts<const G> : Verts<G> {
			};

			template <class G>
			struct Verts<std::reference_wrapper<G>> : Verts<G> {
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
				}
				template <class T>
				static decltype(auto) ephemeral_map(Ref ref, T default_) {
					return Base::ephemeral_map(ref.get(), std::move(default_));
				}
			};

			// Canonical place to read graph edge traits.
			template <class G>
			struct Edges {
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
				}
				template <class T>
				using ephemeral_map_type = typename G::template Ephemeral_edge_map<T>;
				template <class T>
				static ephemeral_map_type<T> ephemeral_map(const G& g, T default_) {
					return g.ephemeral_edge_map(std::move(default_));
				}
			};

			template <class G>
			struct Edges<const G> : Edges<G> {
			};

			template <class G>
			struct Edges<std::reference_wrapper<G>> : Edges<G> {
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
				}
				template <class T>
				static decltype(auto) ephemeral_map(Ref ref, T default_) {
					return Base::ephemeral_map(ref.get(), std::move(default_));
				}
			};

			// Canonical place to read graph outgoing edge traits.
			template <class G>
			struct Out_edges {
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

			template <class G>
			struct Out_edges<const G> : Out_edges<G> {};

			template <class G>
			struct Out_edges<std::reference_wrapper<G>> : Out_edges<G> {
				using Ref = std::reference_wrapper<G>;
				using Base = Out_edges<G>;
				static decltype(auto) range(Ref ref,
					const typename Base::key_type& v) {
					return Base::range(ref.get(), v);
				}
				static decltype(auto) size(Ref ref,
					const typename Base::key_type& v) {
					return Base::size(ref.get(), v);
				}
			};

			// Canonical place to read graph incoming edge traits.
			template <class G>
			struct In_edges {
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

			template <class G>
			struct In_edges<const G> : In_edges<G> {};

			template <class G>
			struct In_edges<std::reference_wrapper<G>> : In_edges<G> {
				using Ref = std::reference_wrapper<G>;
				using Base = In_edges<G>;
				static decltype(auto) range(Ref ref,
					const typename Base::key_type& v) {
					return Base::range(ref.get(), v);
				}
				static decltype(auto) size(Ref ref,
					const typename Base::key_type& v) {
					return Base::size(ref.get(), v);
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
					return Insert_verts<G>::insert(ref.get(),
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
					return Insert_edges<G>::insert(ref.get(),
						std::forward<Args>(args)...);
				}
			};

			template <class Trait>
			struct _trait_key_helper;

			template <class G>
			struct _trait_key_helper<Out_edges<G>> {
				using _edge_traits = Edges<G>;
				using _edge_type = typename _edge_traits::value_type;
				static decltype(auto) key(const G& g, const _edge_type& e) {
					return _edge_traits::tail(g, e);
				}
				static decltype(auto) cokey(const G& g, const _edge_type& e) {
					return _edge_traits::head(g, e);
				}
			};

			template <class G>
			struct _trait_key_helper<In_edges<G>> {
				using _edge_traits = Edges<G>;
				using _edge_type = typename _edge_traits::value_type;
				static decltype(auto) key(const G& g, const _edge_type& e) {
					return _edge_traits::head(g, e);
				}
				static decltype(auto) cokey(const G& g, const _edge_type& e) {
					return _edge_traits::tail(g, e);
				}
			};

			template <class Trait, class G, class E>
			decltype(auto) trait_key(const G& g, E&& e) {
				return _trait_key_helper<Trait>::key(g, std::forward<E>(e));
			}

			template <class Trait, class G, class E>
			decltype(auto) trait_cokey(const G& g, E&& e) {
				return _trait_key_helper<Trait>::cokey(g, std::forward<E>(e));
			}
		}
	}
}
