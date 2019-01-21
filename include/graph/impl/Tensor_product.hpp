#pragma once

#include <utility>
#include <iosfwd>

#include <range/v3/view/cartesian_product.hpp>
#include <range/v3/algorithm/transform.hpp>

#include "traits.hpp"
#include "construct_fn.hpp"
#include "unordered_set.hpp"
#include "unordered_key_map.hpp"

// TODO: Move `tuple_wrapper` to its own file.
namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class... T>
			//using tuple_t = std::tuple<T...>;
			using tuple_t = ranges::v3::common_tuple<T...>;

			template <class... T>
			struct tuple_wrapper {
				using tuple_type = tuple_t<T...>;
				tuple_type _tuple;
				explicit tuple_wrapper(tuple_type&& tuple) :
					_tuple(std::forward<tuple_type>(tuple)) {}
#define GRAPH_IMPL_TUPLE_WRAPPER_BINARY_OP(OP) \
				constexpr auto operator OP(const tuple_wrapper& rhs) const \
					noexcept(noexcept(_tuple OP rhs._tuple)) { \
					return _tuple OP rhs._tuple; \
				}
				GRAPH_IMPL_TUPLE_WRAPPER_BINARY_OP(< )
				GRAPH_IMPL_TUPLE_WRAPPER_BINARY_OP(<=)
				GRAPH_IMPL_TUPLE_WRAPPER_BINARY_OP(> )
				GRAPH_IMPL_TUPLE_WRAPPER_BINARY_OP(>=)
				GRAPH_IMPL_TUPLE_WRAPPER_BINARY_OP(==)
				GRAPH_IMPL_TUPLE_WRAPPER_BINARY_OP(!=)
#undef GRAPH_IMPL_TUPLE_WRAPPER_BINARY_OP
				template <class Char, class Traits>
				friend decltype(auto) operator<<(std::basic_ostream<Char, Traits>& stream, const tuple_wrapper& self) {
					constexpr bool wrap = sizeof...(T) != 1;
					if (wrap) stream << "(";
					auto remaining = sizeof...(T);
					ranges::tuple_for_each(self._tuple,
						[&](const auto& value) {
							stream << value;
							if (--remaining)
								stream << ", ";
						});
					if (wrap) stream << ")";
					return stream;
				}
				using key_type = tuple_wrapper;
				const tuple_wrapper& key() const { return *this; }
			};
		}
	}
}
namespace std {
	template <class... T>
	struct hash<::graph::v1::impl::tuple_wrapper<T...>> {
		using argument_type = ::graph::v1::impl::tuple_wrapper<T...>;
		std::tuple<hash<T>...> _hashers;
		size_t operator()(const argument_type& a) const {
			return ::ranges::tuple_foldl(
				::ranges::tuple_transform(_hashers, a._tuple,
					[](const auto& hasher, const auto& value) {
						return hasher(value);
					}),
				std::size_t{},
				[](std::size_t result, const auto& hash) {
					return result ^ (hash + 0x9e3779b9 + (result << 6) + (result >> 2));
				});
		}
	};
}

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class... G>
			class Tensor_product {
				tuple_t<G...> _g;
				template <class G_>
				static auto Verts(const G_& g) {
					return traits::Verts<std::decay_t<decltype(g)>>{};
				}
				template <class G_>
				static auto Edges(const G_& g) {
					return traits::Edges<std::decay_t<decltype(g)>>{};
				}
			public:
				using Vert = tuple_wrapper<typename traits::Verts<G>::value_type...>;
				using Order = std::common_type_t<int, typename traits::Verts<G>::size_type...>;
				using Edge = tuple_wrapper<typename traits::Edges<G>::value_type...>;
				using Size = std::common_type_t<int, typename traits::Edges<G>::size_type...>;
				explicit Tensor_product(G&&... g) : _g(g...) {}
				auto verts() const {
					return ranges::tuple_apply(
						ranges::view::cartesian_product,
						ranges::tuple_transform(_g,
							[](const auto& g) { return decltype(Verts(g))::range(g); }
						)) | ranges::view::transform(construct<Vert>);
				}
				auto order() const {
					return ranges::tuple_foldl(
						ranges::tuple_transform(_g,
							[](const auto& g) { return decltype(Verts(g))::size(g); }
						), 1, std::multiplies{});
				}
				Vert null_vert() const {
					return Vert(ranges::tuple_transform(_g,
						[](const auto& g) { return decltype(Verts(g))::null(g); }
					));
				}
				auto edges() const {
					return ranges::tuple_apply(
						ranges::view::cartesian_product,
						ranges::tuple_transform(_g,
							[](const auto& g) { return decltype(Edges(g))::range(g); }
					)) | ranges::view::transform(construct<Edge>);
				}
				auto size() const {
					return ranges::tuple_foldl(
						ranges::tuple_transform(_g,
							[](const auto& g) { return decltype(Edges(g))::size(g); }
					), 1, std::multiplies{});
				}
				Edge null_edge() const {
					return Edge(ranges::tuple_transform(_g,
						[](const auto& g) { return decltype(Edges(g))::null(g); }
					));
				}
				auto tail(const Edge& e) const {
					return Vert(ranges::tuple_transform(_g, e._tuple,
						[](const auto& g, const auto &e) {
							return decltype(Edges(g))::tail(g, e);
						}));
				}
				auto head(const Edge& e) const {
					return Vert(ranges::tuple_transform(_g, e._tuple,
						[](const auto& g, const auto &e) {
							return decltype(Edges(g))::head(g, e);
						}));
				}

				using Vert_set = unordered_set<Vert>;
				auto vert_set() const {
					return Vert_set();
				}
				using Ephemeral_vert_set = Vert_set;
				auto ephemeral_vert_set() const {
					return vert_set();
				}
				template <class T>
				using Vert_map = unordered_key_map<Vert, T>;
				template <class T>
				auto vert_map(T default_) const {
					return Vert_map<T>(std::move(default_));
				}
				template <class T>
				using Ephemeral_vert_map = Vert_map<T>;
				template <class T>
				auto ephemeral_vert_map(T default_) const {
					return vert_map(std::move(default_));
				}

				using Edge_set = unordered_set<Edge>;
				auto edge_set() const {
					return Edge_set();
				}
				using Ephemeral_edge_set = Edge_set;
				auto ephemeral_edge_set() const {
					return edge_set();
				}
				template <class T>
				using Edge_map = unordered_key_map<Edge, T>;
				template <class T>
				auto edge_map(T default_) const {
					return Edge_map<T>(std::move(default_));
				}
				template <class T>
				using Ephemeral_edge_map = Edge_map<T>;
				template <class T>
				auto ephemeral_edge_map(T default_) const {
					return edge_map(std::move(default_));
				}

				// TODO: Add support for adjacencies when they are available in all constituent graphs.
			};
		}
	}
}