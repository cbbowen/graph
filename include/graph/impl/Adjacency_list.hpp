#pragma once

#include <vector>
#include <map>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>

#include "unordered_set.hpp"
#include "map_iterator_wrapper.hpp"
#include "tracker.hpp"
#include "construct_fn.hpp"

#define _GRAPH_ADJACENCY_LIST_MUTABLE_HACK 1
#if _GRAPH_ADJACENCY_LIST_MUTABLE_HACK
#	define _GRAPH_ADJACENCY_LIST_VERT_ITERATOR iterator
#	define _GRAPH_ADJACENCY_LIST_VLIST_MUTABLE mutable
#	define _GRAPH_ADJACENCY_LIST_REMOVE_CONST(...) (__VA_ARGS__)
#else
#	define _GRAPH_ADJACENCY_LIST_VERT_ITERATOR const_iterator
#	define _GRAPH_ADJACENCY_LIST_VLIST_MUTABLE
#	define _GRAPH_ADJACENCY_LIST_REMOVE_CONST(...) \
	(_vlist.erase((__VA_ARGS__), (__VA_ARGS__)))
#endif

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class Pair> struct ordered_pair_hasher;
			template <class First, class Second>
			struct ordered_pair_hasher<std::pair<First, Second>> {
				std::size_t operator()(const std::pair<First, Second>& pair) const {
					std::size_t hash = _hashers.first(pair.first);
					hash ^= _hashers.second(pair.second) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
					return hash;
				}
			private:
				std::pair<std::hash<First>, std::hash<Second>> _hashers;
			};

			struct _Adjacency_list {
				using Order = std::size_t;
				using Size = std::size_t;
				using _Degree = std::size_t;
				struct _Vert; // work around the fact that Vert can't be defined yet
				using _elist_type = std::map<Size, _Vert>;
				using _vlist_type = std::map<Order, _elist_type>;
				using Vert = map_iterator_wrapper<typename _vlist_type::_GRAPH_ADJACENCY_LIST_VERT_ITERATOR>;
				struct _Vert : Vert {};
				using _Edge = map_iterator_wrapper<typename _elist_type::const_iterator>;
				using Edge = std::pair<Vert, _Edge>;
				auto verts() const {
					return ranges::view::iota(
							_vlist.begin(), _vlist.end()) |
						ranges::view::transform(construct<Vert>);
				}
				auto order() const noexcept {
					return _vlist.size();
				}
				auto null_vert() const noexcept {
					return Vert{_vlist.end()};
				}
				static auto _vert_edges(const Vert& v) {
					auto&& es = v._it->second;
					return ranges::view::iota(es.begin(), es.end()) |
						ranges::view::transform([v_ = v]
							(typename _elist_type::const_iterator eit){
								return Edge{v_, std::move(eit)};
							});
				}
				static _Degree _degree(const Vert& v) {
					return v._it->second.size();
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
					return Edge{null_vert(), {}};
				}
				static auto _edge_key(const Edge& e) {
					return e.first;
				}
				static auto _edge_cokey(const Edge& e) {
					return e.second._it->second;
				}
				auto insert_vert() {
					return Vert{_vlist.emplace_hint(_vlist.end(), _vlast++, _elist_type{})};
				}
				// precondition: `v` must be unreachable from other vertices
				void erase_vert(const Vert& v) {
#ifndef NDEBUG
					for (auto e : edges())
						if (_edge_cokey(e) == v && _edge_key(e) != v)
							throw std::exception("precondition violated: bad edge adjacent to vertex");
#endif
					_esize -= _degree(v);
					for (auto& m : _vmap_tracker.trackees())
						m._erase(v);
					_vlist.erase(v._it);
				}
				auto _insert_edge(Vert k, Vert v) {
					++_esize;
					auto kit = _GRAPH_ADJACENCY_LIST_REMOVE_CONST(k._it);
					auto&& es = kit->second;
					return Edge{std::move(k),
						es.emplace_hint(es.end(), _elast++, _Vert{std::move(v)})};
				}
				auto erase_edge(const Edge& e) {
					for (auto& m : _emap_tracker.trackees())
						m._erase(e);
					Vert k = _edge_key(e);
					auto kit = _GRAPH_ADJACENCY_LIST_REMOVE_CONST(k._it);
					kit->second.erase(e.second._it);
					--_esize;
				}
				void clear() {
					for (auto& m : _emap_tracker.trackees())
						m._clear();
					_vlist.clear();
					_vlast = 0;
					_esize = _elast = 0;
				}

				template <class T>
				using Vert_map = tracked<persistent_map_iterator_map<Vert, T>, erasable_base<Vert>>;
				template <class T>
				auto vert_map(T default_) const {
					return Vert_map<T>(_vmap_tracker, std::move(default_));
				}

				template <class T>
				using Ephemeral_vert_map = ephemeral_map_iterator_map<Vert, T>;
				template <class T>
				auto ephemeral_vert_map(T default_) const {
					return Ephemeral_vert_map<T>(std::move(default_));
				}

				using Vert_set = tracked<erasable_unordered_set<Vert>, erasable_base<Vert>>;
				auto vert_set() const {
					return Vert_set(_vmap_tracker);
				}
				using Ephemeral_vert_set = unordered_set<Vert>;
				auto ephemeral_vert_set() const {
					return Ephemeral_vert_set();
				}

				template <class T>
				struct _Persistent_edge_map : erasable_base<Edge> {
					using key_type = Edge;
					_Persistent_edge_map(T default_) :
						_map(std::move(default_)) {
					}
					const T& operator()(const Edge& e) const {
						return _map(e.second);
					}
					T& operator[](const Edge& e) {
						return _map[e.second];
					}
					template <class U>
					void assign(const Edge& e, U&& u) {
						_map.assign(e, std::forward<U>(u));
					}
					void _erase(const Edge& e) override {
						_map._erase(e.second);
					}
					void _clear() override {
						_map._clear();
					}
				private:
					persistent_map_iterator_map<_Edge, T> _map;
				};
				template <class T>
				using Edge_map = tracked<_Persistent_edge_map<T>, erasable_base<Edge>>;
				template <class T>
				auto edge_map(T default_) const {
					return Edge_map<T>(_emap_tracker, std::move(default_));
				}

				template <class T>
				struct Ephemeral_edge_map {
					using key_type = Edge;
					Ephemeral_edge_map(T default_) :
						_map(std::move(default_)) {
					}
					const T& operator()(const Edge& e) const {
						return _map(e.second);
					}
					T& operator[](const Edge& e) {
						return _map[e.second];
					}
					template <class U>
					void assign(const Edge& e, U&& u) {
						_map.assign(e, std::forward<U>(u));
					}
				private:
					ephemeral_map_iterator_map<_Edge, T> _map;
				};
				template <class T>
				auto ephemeral_edge_map(T default_) const {
					return Ephemeral_edge_map<T>(std::move(default_));
				}

				// relies on `std::pair::operator==` only checking `std::pair::second` for equality if `std::pair::first` matches; otherwise behavior is undefined because iterators from different containers are incomparable.  Fortunately, the standard indicates this is the required behavior.
				using Edge_set = tracked<
					erasable_unordered_set<Edge, ordered_pair_hasher<Edge>>,
					erasable_base<Edge>>;
				auto edge_set() const {
					return Edge_set(_emap_tracker);
				}
				using Ephemeral_edge_set = unordered_set<Edge, ordered_pair_hasher<Edge>>;
				auto ephemeral_edge_set() const {
					return Ephemeral_edge_set();
				}
			private:
				_GRAPH_ADJACENCY_LIST_VLIST_MUTABLE _vlist_type _vlist;
				Order _vlast = 0;
				Size _esize = 0, _elast = 0;
				tracker<erasable_base<Vert>> _vmap_tracker;
				tracker<erasable_base<Edge>> _emap_tracker;
			};

			struct Out_adjacency_list : _Adjacency_list {
				using _base_type = _Adjacency_list;
				using _base_type::_base_type;
				using Vert = typename _base_type::Vert;
				using Edge = typename _base_type::Edge;
				using Out_degree = typename _base_type::_Degree;
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
				auto insert_edge(Vert s, Vert t) {
					return _base_type::_insert_edge(std::move(s), std::move(t));
				}
			};

			struct In_adjacency_list : _Adjacency_list {
				using _base_type = _Adjacency_list;
				using _base_type::_base_type;
				using Vert = typename _base_type::Vert;
				using Edge = typename _base_type::Edge;
				using In_degree = typename _base_type::_Degree;
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
				auto insert_edge(Vert s, Vert t) {
					return _base_type::_insert_edge(std::move(t), std::move(s));
				}
			};
		}
	}
}
