#pragma once

#include <vector>
#include <map>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/to_container.hpp>

#include "unordered_set.hpp"
#include "map_iterator_wrapper.hpp"
#include "tracker.hpp"
#include "construct_fn.hpp"
#include "exceptions.hpp"
#include "Edge_list.hpp"

// Ideally, there would be a good way to get an iterator from const_iterator and a mutable container (better than a zero-length erase).  Since there is not, we have to make our container mutable so we can get iterators to it in const contexts.
#define GRAPH_V1_ADJACENCY_LIST_MUTABLE_HACK 1
#if GRAPH_V1_ADJACENCY_LIST_MUTABLE_HACK
#	define GRAPH_V1_ADJACENCY_LIST_VERT_ITERATOR iterator
#	define GRAPH_V1_ADJACENCY_LIST_VLIST_MUTABLE mutable
#	define GRAPH_V1_ADJACENCY_LIST_REMOVE_CONST(...) (__VA_ARGS__)
#else
#	define GRAPH_V1_ADJACENCY_LIST_VERT_ITERATOR const_iterator
#	define GRAPH_V1_ADJACENCY_LIST_VLIST_MUTABLE
#	define GRAPH_V1_ADJACENCY_LIST_REMOVE_CONST(...) \
	(_vlist.erase((__VA_ARGS__), (__VA_ARGS__)))
#endif

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class Pair>
			struct ordered_pair_hasher {
				std::size_t operator()(const Pair& pair) const {
					std::size_t hash = _hashers.first(pair.first);
					hash ^= _hashers.second(pair.second) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
					return hash;
				}
			private:
				std::pair<std::hash<typename Pair::first_type>, std::hash<typename Pair::second_type>> _hashers;
			};

			template <template <class, class, class...> class Map_>
			struct Adjacency_list_base {
				using Order = std::size_t;
				using Size = std::size_t;
				using _degree_type = std::size_t;
				struct _vert_type; // work around the fact that Vert can't be defined yet
				using _elist_type = Map_<Size, _vert_type>;
				using _vlist_type = Map_<Order, _elist_type>;
				using Vert = map_iterator_wrapper<typename _vlist_type::GRAPH_V1_ADJACENCY_LIST_VERT_ITERATOR>;
				struct _vert_type : Vert {};
				using _edge_type = map_iterator_wrapper<typename _elist_type::const_iterator>;
				struct Edge : std::pair<Vert, _edge_type> {
					using _base_type = std::pair<Vert, _edge_type>;
					using _base_type::_base_type;
					template <class Char, class Traits>
					friend decltype(auto) operator<<(std::basic_ostream<Char, Traits>& s, const Edge& e) {
						return s << e.second;
					}
				};
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
				static _degree_type _degree(const Vert& v) {
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
#if GRAPH_CHECK_PRECONDITIONS
					for (auto e : edges())
						check_precondition(_edge_cokey(e) != v || _edge_key(e) == v,
							"bad edge adjacent to vertex");
#endif
					_esize -= _degree(v);
					for (auto& m : _vmap_tracker.trackees())
						m._erase(v);
					_vlist.erase(v._it);
				}
				auto _insert_edge(Vert k, Vert v) {
					++_esize;
					auto kit = GRAPH_V1_ADJACENCY_LIST_REMOVE_CONST(k._it);
					auto&& es = kit->second;
					return Edge{std::move(k),
						es.emplace_hint(es.end(), _elast++, _vert_type{std::move(v)})};
				}
				auto erase_edge(const Edge& e) {
					for (auto& m : _emap_tracker.trackees())
						m._erase(e);
					Vert k = _edge_key(e);
					auto kit = GRAPH_V1_ADJACENCY_LIST_REMOVE_CONST(k._it);
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

				using _vmap_tracker_type = tracker<erasable_base<Vert>>;

				template <class T>
				using Vert_map = tracked<persistent_map_iterator_map<Vert, T>, _vmap_tracker_type>;
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

				using Vert_set = tracked<erasable_unordered_set<Vert>, _vmap_tracker_type>;
				auto vert_set() const {
					return Vert_set(_vmap_tracker);
				}
				using Ephemeral_vert_set = unordered_set<Vert>;
				auto ephemeral_vert_set() const {
					return Ephemeral_vert_set();
				}

				using _emap_tracker_type = tracker<erasable_base<Edge>>;

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
						_map.assign(e.second, std::forward<U>(u));
					}
					template <class U>
					T exchange(const Edge& e, U&& u) {
						return _map.exchange(e.second, std::forward<U>(u));
					}
					void _erase(const Edge& e) override {
						_map._erase(e.second);
					}
					void _clear() override {
						_map._clear();
					}
				private:
					persistent_map_iterator_map<_edge_type, T> _map;
				};
				template <class T>
				using Edge_map = tracked<_Persistent_edge_map<T>, _emap_tracker_type>;
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
						_map.assign(e.second, std::forward<U>(u));
					}
					template <class U>
					T exchange(const Edge& e, U&& u) {
						return _map.exchange(e.second, std::forward<U>(u));
					}
				private:
					ephemeral_map_iterator_map<_edge_type, T> _map;
				};
				template <class T>
				auto ephemeral_edge_map(T default_) const {
					return Ephemeral_edge_map<T>(std::move(default_));
				}

				// relies on `std::pair::operator==` only checking `std::pair::second` for equality if `std::pair::first` matches; otherwise behavior is undefined because iterators from different containers are incomparable.  Fortunately, the standard indicates this is the required behavior.
				using Edge_set = tracked<
					erasable_unordered_set<Edge, ordered_pair_hasher<Edge>>,
					_emap_tracker_type>;
				auto edge_set() const {
					return Edge_set(_emap_tracker);
				}
				using Ephemeral_edge_set = unordered_set<Edge, ordered_pair_hasher<Edge>>;
				auto ephemeral_edge_set() const {
					return Ephemeral_edge_set();
				}
			private:
				GRAPH_V1_ADJACENCY_LIST_VLIST_MUTABLE _vlist_type _vlist;
				Order _vlast = 0;
				Size _esize = 0, _elast = 0;
				_vmap_tracker_type _vmap_tracker;
				_emap_tracker_type _emap_tracker;
			};

			template <template <class, class, class...> class Map_ = std::map>
			struct Out_adjacency_list : Adjacency_list_base<Map_> {
				using _base_type = Adjacency_list_base<Map_>;
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
				auto insert_edge(Vert s, Vert t) {
					return _base_type::_insert_edge(std::move(s), std::move(t));
				}
			};

			static_assert(traits::has_out_edges<Out_adjacency_list<>>);

			template <template <class, class, class...> class Map_ = std::map>
			struct In_adjacency_list : Adjacency_list_base<Map_> {
				using _base_type = Adjacency_list_base<Map_>;
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
				auto insert_edge(Vert s, Vert t) {
					return _base_type::_insert_edge(std::move(t), std::move(s));
				}
			};

			static_assert(traits::has_in_edges<In_adjacency_list<>>);

			template <class Order_ = std::size_t, class Size_ = std::size_t>
			struct Bi_adjacency_list : Edge_list<Order_, Size_> {
				using _base_type = Edge_list<Order_, Size_>;
				using _base_type::_base_type;
				using _elist_type = typename _base_type::Edge_set;
				using _alist_type = typename _base_type::template Vert_map<std::pair<_elist_type, _elist_type>>;
				using Vert = typename _base_type::Vert;
				using Edge = typename _base_type::Edge;
				using Out_degree = typename _elist_type::size_type;
				using In_degree = typename _elist_type::size_type;
				Bi_adjacency_list() :
					_alist(this->vert_map(std::make_pair(
						this->edge_set(), this->edge_set()
					))) {
				}
				auto out_edges(const Vert& v) const {
					return ranges::view::all(_alist(v).first);
				}
				auto out_degree(const Vert& v) const {
					return _alist(v).first.size();
				}
				auto in_edges(const Vert& v) const {
					return ranges::view::all(_alist(v).second);
				}
				auto in_degree(const Vert& v) const {
					return _alist(v).second.size();
				}
				auto insert_edge(Vert s, Vert t) {
					auto e = _base_type::insert_edge(std::move(s), t);
					_alist[s].first.insert(e);
					_alist[t].second.insert(e);
					return e;
				}
				void erase_vert(const Vert& v) {
					auto out_edges = std::move(_alist[v].first)._untracked()._raw();
					auto in_edges = std::move(_alist[v].second)._untracked()._raw();
					for (auto e : ranges::to_vector(std::move(out_edges))) {
						// This is necessary to handle to self-edges
						in_edges.erase(e);
						_base_type::erase_edge(e);
					}
					for (auto e : ranges::to_vector(std::move(in_edges)))
						_base_type::erase_edge(e);
					_base_type::erase_vert(v);
				}
				void erase_edge(const Edge& e) {
					bool result0 = _alist[this->tail(e)].first.erase(e);
					bool result1 = _alist[this->head(e)].second.erase(e);
					assert(result0 && result1);
					_base_type::erase_edge(e);
				}
			private:
				_alist_type _alist;
			};

			static_assert(traits::has_bi_edges<Bi_adjacency_list<>>);
		}
	}
}

#undef GRAPH_V1_ADJACENCY_LIST_MUTABLE_HACK
#undef GRAPH_V1_ADJACENCY_LIST_VERT_ITERATOR
#undef GRAPH_V1_ADJACENCY_LIST_VLIST_MUTABLE
#undef GRAPH_V1_ADJACENCY_LIST_REMOVE_CONST
