#pragma once

#include <range/v3/view/all.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>

#include "atomic_list.hpp"
#include "pointer_wrapper.hpp"
#include "unordered_key_map.hpp"
#include "unordered_set.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <template <class> class Atomic_container = atomic_list>
			struct _Atomic_adjacency_list {
				struct _enode;
				using Edge = pointer_wrapper<_enode>;
				using _vnode = Atomic_container<Edge>;
				using Vert = pointer_wrapper<_vnode>;
				struct _enode {
					_enode(Vert key, Vert cokey) : _key(key), _cokey(cokey) {}
					Vert _key, _cokey;
				};

				using _vlist_type = Atomic_container<_vnode>;
				using _elist_type = Atomic_container<_enode>;

				using Order = typename _vlist_type::size_type;
				using Size = typename _elist_type::size_type;
				using _Degree = typename _vnode::size_type;

				auto verts() const {
					return ranges::view::all(_vlist) |
						//ranges::view::transform(wrap_pointer<_vnode>);
						ranges::view::transform([](auto& v){ return Vert{&v}; });
				}
				auto order() const noexcept {
					return _vlist.conservative_size();
				}
				auto null_vert() const noexcept {
					return Vert{};
				}
				static auto _vert_edges(const Vert& v) {
					return ranges::view::all(*v);
				}
				static _Degree _degree(const Vert& v) {
					return v->conservative_size();
				}
				auto edges() const {
					return verts() |
						ranges::view::transform(&_vert_edges) |
						ranges::view::join;
				}
				auto size() const noexcept {
					return _elist.conservative_size();
				}
				auto null_edge() const noexcept {
					return Edge{};
				}
				static auto _edge_key(const Edge& e) {
					return e->_key;
				}
				static auto _edge_cokey(const Edge& e) {
					return e->_cokey;
				}
				auto insert_vert() {
					return Vert{_vlist.emplace()};
				}
				auto _insert_edge(Vert k, Vert v) {
					auto e = Edge{_elist.emplace(k, v)};
					k->emplace(e);
					return e;
				}

				template <class T>
				using Vert_map = unordered_key_map<Vert, T>;
				template <class T>
				auto vert_map(T default_) const {
					return Vert_map<T>(std::move(default_));
				}  // LCOV_EXCL_LINE (unreachable)
				template <class T>
				using Ephemeral_vert_map = Vert_map<T>;
				template <class T>
				auto ephemeral_vert_map(T default_) const {
					return vert_map(std::move(default_));
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
				using Edge_map = unordered_key_map<Edge, T>;
				template <class T>
				auto edge_map(T default_) const {
					return Edge_map<T>(std::move(default_));
				}  // LCOV_EXCL_LINE (unreachable)
				template <class T>
				using Ephemeral_edge_map = Edge_map<T>;
				template <class T>
				auto ephemeral_edge_map(T default_) const {
					return edge_map(std::move(default_));
				}

				using Edge_set = unordered_set<Edge>;
				auto edge_set() const {
					return Edge_set();
				}
				using Ephemeral_edge_set = Edge_set;
				auto ephemeral_edge_set() const {
					return edge_set();
				}
			private:
				// Vertices are non-const pointers so that insert_edge can be nice, which means this needs to be mutable.
				mutable _vlist_type _vlist;
				_elist_type _elist;
			};

			struct Atomic_out_adjacency_list : _Atomic_adjacency_list<> {
				using _base_type = _Atomic_adjacency_list<>;
				using _base_type::_base_type;
				using Vert = typename _base_type::Vert;
				using Edge = typename _base_type::Edge;
				using Out_degree = typename _base_type::_Degree;
				static inline auto out_edges(const Vert& v) {
					return _base_type::_vert_edges(v);
				}
				static inline Out_degree out_degree(const Vert& v) {
					return _base_type::_degree(v);
				}
				static inline auto tail(const Edge& e) {
					return _base_type::_edge_key(e);
				}
				static inline auto head(const Edge& e) {
					return _base_type::_edge_cokey(e);
				}
				inline auto insert_edge(Vert s, Vert t) {
					return _base_type::_insert_edge(std::move(s), std::move(t));
				}
			};

			struct Atomic_in_adjacency_list : _Atomic_adjacency_list<> {
				using _base_type = _Atomic_adjacency_list<>;
				using _base_type::_base_type;
				using Vert = typename _base_type::Vert;
				using Edge = typename _base_type::Edge;
				using In_degree = typename _base_type::_Degree;
				static inline auto in_edges(const Vert& v) {
					return _base_type::_vert_edges(v);
				}
				static inline In_degree in_degree(const Vert& v) {
					return _base_type::_degree(v);
				}
				static inline auto tail(const Edge& e) {
					return _base_type::_edge_cokey(e);
				}
				static inline auto head(const Edge& e) {
					return _base_type::_edge_key(e);
				}
				inline auto insert_edge(Vert s, Vert t) {
					return _base_type::_insert_edge(std::move(t), std::move(s));
				}
			};
		}
	}
}
