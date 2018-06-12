#pragma once

#include <deque>
#include <memory>

#include <range/v3/view/all.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>

#include "pointer_wrapper.hpp"
#include "unordered_set.hpp"
#include "unordered_key_map.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class VData, class EData,
				template <class...> class VContainer_,
				template <class...> class EContainer_>
			class Stable_node_list_base {
				struct _vert_type;
				struct _edge_type : private std::tuple<EData> {
					using base_type = std::tuple<EData>;
					_edge_type(EData&& data, const _vert_type *key, const _vert_type *cokey) :
						base_type{std::move(data)}, _key(key), _cokey(cokey) {
					}
					const _vert_type *_key, *_cokey;
					const EData& _data() const {
						return std::get<0>(*static_cast<const base_type*>(this));
					}
					EData& _data() {
						return std::get<0>(*static_cast<base_type*>(this));
					}
				};
				using _alist_type = EContainer_<_edge_type>;
				struct _vert_type : private std::tuple<VData> {
					using base_type = std::tuple<VData>;
					_vert_type(VData&& data) :
						base_type(std::move(data)) {
					}
					mutable _alist_type _alist;
					const VData& _data() const {
						return std::get<0>(*static_cast<const base_type*>(this));
					}
					VData& _data() {
						return std::get<0>(*static_cast<base_type*>(this));
					}
				};
				using _vlist_type = VContainer_<std::unique_ptr<_vert_type>>;
			public:
				using Vert = pointer_wrapper<const _vert_type>;
				using Edge = pointer_wrapper<const _edge_type>;
			protected:
				using _degree_type = typename _alist_type::size_type;
				static auto _vert_edges(const Vert& v) {
					return ranges::view::all(v->_alist) |
						ranges::view::transform(
							[](const auto& e){ return Edge{&e}; });
				}
				static _degree_type _degree(const Vert& v) {
					return v->_alist.size();
				}
				static auto _edge_key(const Edge& e) {
					return Vert{e->_key};
				}
				static auto _edge_cokey(const Edge& e) {
					return Vert{e->_cokey};
				}
				auto _insert_edge(Vert k, Vert v, EData&& data) {
					++_esize;
					k->_alist.emplace_back(std::move(data), k.key(), v.key());
					return Edge{&k->_alist.back()};
				}
			public:
				using Order = typename _vlist_type::size_type;
				using Size = decltype(std::declval<Order>() * std::declval<_degree_type>());
				auto verts() const {
					return ranges::view::all(_vlist) |
						ranges::view::transform(
							[](const auto& v){ return Vert(v.get()); });
				}
				auto order() const noexcept {
					return _vlist.size();
				}
				auto null_vert() const noexcept {
					return Vert{};
				}
				auto edges() const {
					return verts() |
						ranges::view::transform(&_vert_edges) |
						ranges::view::join;
				}
				auto size() const noexcept {
					return _esize;
				}
				auto null_edge() const noexcept {
					return Edge{};
				}
				auto insert_vert(VData data = {}) {
					auto p = std::make_unique<_vert_type>(std::move(data));
					auto v = Vert{p.get()};
					_vlist.push_back(std::move(p));
					return v;
				}

				template <class T>
				using Vert_map = unordered_key_map<Vert, T>;
				template <class T>
				auto vert_map(T default_) const {
					return Vert_map<T>(std::move(default_));
				} // LCOV_EXCL_LINE (unreachable)

				template <class T>
				using Ephemeral_vert_map = Vert_map<T>;
				template <class T>
				auto ephemeral_vert_map(T default_) const {
					return vert_map(std::move(default_));
				}

				using Vert_set = unordered_set<Vert>;
				auto vert_set() const {
					return Vert_set{};
				}
				using Ephemeral_vert_set = Vert_set;
				auto ephemeral_vert_set() const {
					return vert_set();
				}

				template <class T>
				using Edge_map = unordered_key_map<Edge, T>;
				template <class T>
				auto edge_map(T default_) const {
					return Edge_map<T>(std::move(default_));
				} // LCOV_EXCL_LINE (unreachable)

				template <class T>
				using Ephemeral_edge_map = Edge_map<T>;
				template <class T>
				auto ephemeral_edge_map(T default_) const {
					return edge_map(std::move(default_));
				}

				using Edge_set = unordered_set<Edge>;
				auto edge_set() const {
					return Edge_set{};
				}
				using Ephemeral_edge_set = Edge_set;
				auto ephemeral_edge_set() const {
					return edge_set();
				}

				const VData& vert_data(Vert v) const {
					return v->_data();
				}
				VData& vert_data(Vert v) {
					return const_cast<_vert_type *>(v.key())->_data();
				}

				const EData& edge_data(Edge e) const {
					return e->_data();
				}
				EData& edge_data(Edge e) {
					return const_cast<_edge_type *>(e.key())->_data();
				}
			private:
				_vlist_type _vlist;
				Size _esize = 0;
			};

			template <class VData, class EData,
				template <class, class...> class VContainer_ = std::deque,
				template <class, class...> class EContainer_ = VContainer_>
			struct Stable_out_node_list : Stable_node_list_base<VData, EData, VContainer_, EContainer_> {
				using _base_type = Stable_node_list_base<VData, EData, VContainer_, EContainer_>;
				using _base_type::_base_type;
				using Vert = typename _base_type::Vert;
				using Edge = typename _base_type::Edge;
				using Out_degree = typename _base_type::_degree_type;
				static auto out_edges(const Vert& v) {
					return _base_type::_vert_edges(v);
				}
				static Out_degree out_degree(const Vert& v) {
					return _base_type::_degree(v);
				}
				static auto tail(const Edge& e) {
					return _base_type::_edge_key(e);
				}
				static auto head(const Edge& e) {
					return _base_type::_edge_cokey(e);
				}
				auto insert_edge(Vert s, Vert t, EData data = {}) {
					return _base_type::_insert_edge(std::move(s), std::move(t), std::move(data));
				}
			};

			static_assert(traits::has_out_edges<Stable_out_node_list<int, int>>);

			template <class VData = std::tuple<>, class EData = std::tuple<>,
				template <class, class...> class VContainer_ = std::deque,
				template <class, class...> class EContainer_ = VContainer_>
			struct Stable_in_node_list : Stable_node_list_base<VData, EData, VContainer_, EContainer_> {
				using _base_type = Stable_node_list_base<VData, EData, VContainer_, EContainer_>;
				using _base_type::_base_type;
				using Vert = typename _base_type::Vert;
				using Edge = typename _base_type::Edge;
				using In_degree = typename _base_type::_degree_type;
				static auto in_edges(const Vert& v) {
					return _base_type::_vert_edges(v);
				}
				static In_degree in_degree(const Vert& v) {
					return _base_type::_degree(v);
				}
				static auto tail(const Edge& e) {
					return _base_type::_edge_cokey(e);
				}
				static auto head(const Edge& e) {
					return _base_type::_edge_key(e);
				}
				auto insert_edge(Vert s, Vert t, EData data = {}) {
					return _base_type::_insert_edge(std::move(t), std::move(s), std::move(data));
				}
			};

			static_assert(traits::has_in_edges<Stable_in_node_list<int, int>>);
		}
	}
}
