#pragma once

#include <range/v3/view/all.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>

// This is significantly slower for graph construction, but allows the use of contiguous maps which allow parallel access of values with different keys (after calling `reserve`).  Given this tradeoff, it probably makes sense to add a Contiguous_atomic_adjacency_list.  Such a structure could supply its own `reserve_verts` and `reserve_edges` methods and use (contiguous) maps instead of pointers.  This arrangement would be more difficult to use correctly, but could yield significant performance gains in contentious code.
#define GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_USE_CONTIGUOUS_MAPS 0
#if GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_USE_CONTIGUOUS_MAPS
#	define GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_WRAPPER node_pointer_wrapper
#	define GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_DECLARE_NODE_MEMBERS std::size_t _index
#	define GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_VNODE_CTOR_ARGS _vnext.fetch_add(1, std::memory_order_relaxed)
#	define GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_ENODE_CTOR_ARGS _enext.fetch_add(1, std::memory_order_relaxed),
	// TODO: Use ephemeral_contiguous_key_map where applicable
#	define GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_DECLARE_MAP_TYPE persistent_contiguous_key_map
#	define GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_DECLARE_SET_TYPE unordered_set
#	include "contiguous_key_map.hpp"
#	include "unordered_set.hpp"
#else
#	define GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_WRAPPER pointer_wrapper
#	define GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_DECLARE_NODE_MEMBERS
#	define GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_VNODE_CTOR_ARGS
#	define GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_ENODE_CTOR_ARGS
#	define GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_DECLARE_MAP_TYPE unordered_key_map
#	define GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_DECLARE_SET_TYPE unordered_set
#	include "unordered_key_map.hpp"
#	include "unordered_set.hpp"
#endif

#include "atomic_list.hpp"
#include "pointer_wrapper.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
#if GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_USE_CONTIGUOUS_MAPS
			template <class T>
			struct node_pointer_wrapper {
				using key_type = std::size_t;
				using flag_type = T *;
				using this_type = node_pointer_wrapper;
				node_pointer_wrapper(T *p) noexcept : _p(std::move(p)) {}
				node_pointer_wrapper(key_type, flag_type p) noexcept : this_type(std::move(p)) {}
				node_pointer_wrapper(T& r) noexcept : this_type(&r) {}
				node_pointer_wrapper() noexcept : this_type(nullptr) {}
				key_type key() const { return _p->_index; }
				key_type flag() const { return _p; }
				auto operator ==(const this_type& other) const { return _p == other._p; }
				auto operator !=(const this_type& other) const { return _p != other._p; }
				auto operator <(const this_type& other) const { return _p < other._p; }
				auto operator <=(const this_type& other) const { return _p <= other._p; }
				auto operator >(const this_type& other) const { return _p > other._p; }
				auto operator >=(const this_type& other) const { return _p >= other._p; }
				T& operator*() const { return *_p; }
				T *operator->() const { return _p; }
				template <class Char, class Traits>
				friend decltype(auto) operator<<(std::basic_ostream<Char, Traits>& s, const this_type& x) {
					return s << x.key();
				}
			private:
				friend struct ::std::hash<this_type>;
				T *_p;
			};
#endif
			template <template <class> class Atomic_container = atomic_list>
			struct _Atomic_adjacency_list {
				struct _enode;
				using Edge = GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_WRAPPER<_enode>;
				using _alist_type = Atomic_container<Edge>;
				struct _vnode {
					GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_DECLARE_NODE_MEMBERS;
					_alist_type _alist;
				};
				using Vert = GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_WRAPPER<_vnode>;
				struct _enode {
					GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_DECLARE_NODE_MEMBERS;
					Vert _key, _cokey;
				};

				using _vlist_type = Atomic_container<_vnode>;
				using _elist_type = Atomic_container<_enode>;

				using Order = typename _vlist_type::size_type;
				using Size = typename _elist_type::size_type;
				using _Degree = typename _alist_type::size_type;

				auto verts() const {
					return ranges::view::all(_vlist) |
						ranges::view::transform([](auto& v){ return Vert{&v}; });
				}
				auto order() const noexcept {
					return _vlist.conservative_size();
				}
				auto null_vert() const noexcept {
					return Vert{};
				}
				static auto _vert_edges(const Vert& v) {
					return ranges::view::all(v->_alist);
				}
				static _Degree _degree(const Vert& v) {
					return v->_alist.conservative_size();
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
					return Vert{_vlist.emplace(GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_VNODE_CTOR_ARGS)};
				}
				auto _insert_edge(Vert k, Vert v) {
					auto e = Edge{_elist.emplace(GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_ENODE_CTOR_ARGS k, v)};
					k->_alist.emplace(e);
					return e;
				}

				template <class T>
				using Vert_map = GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_DECLARE_MAP_TYPE<Vert, T>;
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

				using Vert_set = GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_DECLARE_SET_TYPE<Vert>;
				auto vert_set() const {
					return Vert_set();
				}
				using Ephemeral_vert_set = Vert_set;
				auto ephemeral_vert_set() const {
					return vert_set();
				}

				template <class T>
				using Edge_map = GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_DECLARE_MAP_TYPE<Edge, T>;
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

				using Edge_set = GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_DECLARE_SET_TYPE<Edge>;
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
#if GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_USE_CONTIGUOUS_MAPS
				std::atomic<std::size_t> _vnext{0}, _enext{0};
#endif
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

#if GRAPH_V1_IMPL_ATOMIC_ADJACENCY_LIST_USE_CONTIGUOUS_MAPS
namespace std {
	template <class T>
	struct hash<::graph::v1::impl::node_pointer_wrapper<T>> {
		using argument_type = ::graph::v1::impl::node_pointer_wrapper<T>;
		auto operator()(const argument_type& a) const {
			// We have a couple of options here.  We could hash the pointer or we could hash the key, since both are unique.  The key produces fewer collisions but requires an extra indirection, so doesn't seem to be worthwhile.
			return _inner_hash(a._p);
		}
	private:
		using _inner_hash_type = hash<T *>;
		_inner_hash_type _inner_hash;
	};
}
#endif
