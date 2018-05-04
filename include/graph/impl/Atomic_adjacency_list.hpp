#pragma once

#include <range/v3/view/all.hpp>

#include "atomic_list.hpp"
#include "Atomic_edge_list.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class Adjacency, template <class> class Atomic_container = atomic_list>
			struct Atomic_adjacency_list_base :
				Atomic_edge_list<> {
				static_assert(std::is_same_v<Adjacency, traits::Out> || std::is_same_v<Adjacency, traits::In>);
				using _base_type = Atomic_edge_list<>;

				using Order = typename _base_type::Order;
				using Vert = typename _base_type::Vert;

				using _alist_type = Atomic_container<Edge>;
				using _vlist_type = std::vector<_alist_type>;
				using _degree_type = typename _alist_type::size_type;

				void reserve_verts(Order capacity) {
					_base_type::reserve_verts(capacity);
					_vlist.resize(this->vert_capacity());
				}
				auto insert_vert() {
					auto v = _base_type::insert_vert();
					if (v.key() >= _vlist.size()) {
						assert(_vlist.size() == v.key());
						_vlist.emplace_back();
					}
					return v;
				}
				auto _insert_adjacency(Vert s, Vert t, Edge e) {
					auto kk = std::is_same_v<Adjacency, traits::Out> ? s.key() : t.key();
					_vlist[kk].emplace(e);
					return e;
				}
				auto atomic_insert_edge(Vert s, Vert t) {
					return _insert_adjacency(s, t,
						_base_type::atomic_insert_edge(s, t));
				}
				auto insert_edge(Vert s, Vert t) {
					return _insert_adjacency(s, t,
						_base_type::insert_edge(s, t));
				}
				auto _vert_edges(Vert k) const {
					return ranges::view::all(_vlist[k.key()]);
				}
				auto _vert_degree(Vert k) const {
					return _vlist[k.key()].conservative_size();
				}
			private:
				_vlist_type _vlist;
			};

			struct Atomic_out_adjacency_list :
				Atomic_adjacency_list_base<traits::Out> {
				using _base_type = Atomic_adjacency_list_base<traits::Out>;
				using _base_type::_base_type;
				using Vert = typename _base_type::Vert;
				using Edge = typename _base_type::Edge;
				using Out_degree = typename _base_type::_degree_type;
				inline auto out_edges(const Vert& v) const {
					return _base_type::_vert_edges(v);
				}
				inline Out_degree out_degree(const Vert& v) const {
					return _base_type::_vert_degree(v);
				}
			};

			struct Atomic_in_adjacency_list :
				Atomic_adjacency_list_base<traits::In> {
				using _base_type = Atomic_adjacency_list_base<traits::In>;
				using _base_type::_base_type;
				using Vert = typename _base_type::Vert;
				using Edge = typename _base_type::Edge;
				using In_degree = typename _base_type::_degree_type;
				inline auto in_edges(const Vert& v) const {
					return _base_type::_vert_edges(v);
				}
				inline In_degree in_degree(const Vert& v) const {
					return _base_type::_vert_degree(v);
				}
			};
		}
	}
}
