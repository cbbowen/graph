#pragma once

#include <unordered_map>
#include <range/v3/iterator_range.hpp>
#include <range/v3/view/transform.hpp>

#include "Stable_edge_list.hpp"
#include "get_fn.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class _Order = std::size_t, class _Size = std::size_t>
			struct Stable_out_adjacency_list :
				Stable_edge_list<_Order, _Size> {
				using _base_type = Stable_edge_list<_Order, _Size>;
				using Vert = typename _base_type::Vert;
				using Edge = typename _base_type::Edge;
				using _alist_type = std::unordered_multimap<Vert, Edge>;
				using Out_degree = typename _alist_type::size_type;
				auto out_edges(const Vert& v) const {
					using _alist_range = ranges::iterator_range<
						typename _alist_type::const_iterator>;
					return _alist_range(_alist.equal_range(v)) |
						ranges::view::transform(get<1>);
				}
				auto out_degree(const Vert& v) const {
					return _alist.count(v);
				}
				auto insert_edge(Vert s, Vert t) {
					auto e = _base_type::insert_edge(s, std::move(t));
					_alist.emplace(std::move(s), e);
					return e;
				}
			private:
				_alist_type _alist;
			};
			template <class _Order = std::size_t, class _Size = std::size_t>
			struct Stable_in_adjacency_list :
				Stable_edge_list<_Order, _Size> {
				using _base_type = Stable_edge_list<_Order, _Size>;
				using Vert = typename _base_type::Vert;
				using Edge = typename _base_type::Edge;
				using _alist_type = std::unordered_multimap<Vert, Edge>;
				using In_degree = typename _alist_type::size_type;
				auto in_edges(const Vert& v) const {
					using _alist_range = ranges::iterator_range<
						typename _alist_type::const_iterator>;
					return _alist_range(_alist.equal_range(v)) |
						ranges::view::transform(get<1>);
				}
				auto in_degree(const Vert& v) const {
					return _alist.count(v);
				}
				auto insert_edge(Vert s, Vert t) {
					auto e = _base_type::insert_edge(std::move(s), t);
					_alist.emplace(std::move(t), e);
					return e;
				}
			private:
				_alist_type _alist;
			};
		}
	}
}
