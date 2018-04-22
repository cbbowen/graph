#pragma once

#include "Stable_edge_list.hpp"

#define GRAPH_V1_STABLE_ADJACENCY_LIST_USE_MULTIMAP 0
#if GRAPH_V1_STABLE_ADJACENCY_LIST_USE_MULTIMAP
#	include <unordered_map>
#	include <range/v3/iterator_range.hpp>
#	include <range/v3/view/transform.hpp>
#	include "get_fn.hpp"
#	define GRAPH_V1_STABLE_ADJACENCY_LIST_TYPE std::unordered_multimap<Vert, Edge>
#	define GRAPH_V1_STABLE_ADJACENCY_LIST_DEGREE_TYPE typename GRAPH_V1_STABLE_ADJACENCY_LIST_TYPE::size_type
#	define GRAPH_V1_STABLE_ADJACENCY_LIST_DEGREE(v, alist) ((alist).count((v)))
#	define GRAPH_V1_STABLE_ADJACENCY_LIST_RANGE(v, alist) \
		ranges::iterator_range<typename GRAPH_V1_STABLE_ADJACENCY_LIST_TYPE::const_iterator>( \
			(alist).equal_range((v))) | ranges::view::transform(get<1>)
#	define GRAPH_V1_STABLE_ADJACENCY_LIST_INSERT_EDGE(k, e, alist) (alist).emplace((k), (e))
#else
#	include <vector>
#	include <range/v3/view/all.hpp>
#	include "get_fn.hpp"
#	define GRAPH_V1_STABLE_ADJACENCY_LIST_TYPE typename traits::Verts<_base_type>::template map_type<std::vector<Edge>>
#	define GRAPH_V1_STABLE_ADJACENCY_LIST_DEGREE_TYPE typename std::vector<Edge>::size_type
#	define GRAPH_V1_STABLE_ADJACENCY_LIST_DEGREE(v, alist) ((alist)((v)).size())
#	define GRAPH_V1_STABLE_ADJACENCY_LIST_RANGE(v, alist) ranges::view::all((alist)((v)))
#	define GRAPH_V1_STABLE_ADJACENCY_LIST_INSERT_EDGE(k, e, alist) (alist)[(k)].push_back((e))
#endif

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class Order = std::size_t, class Size = std::size_t>
			struct Stable_out_adjacency_list :
				Stable_edge_list<Order, Size> {
				using _base_type = Stable_edge_list<Order, Size>;
				using Vert = typename _base_type::Vert;
				using Edge = typename _base_type::Edge;
				using _alist_type = GRAPH_V1_STABLE_ADJACENCY_LIST_TYPE;
				using Out_degree = GRAPH_V1_STABLE_ADJACENCY_LIST_DEGREE_TYPE;
				auto out_edges(const Vert& v) const {
					return GRAPH_V1_STABLE_ADJACENCY_LIST_RANGE(v, _alist);
				}
				auto out_degree(const Vert& v) const {
					return GRAPH_V1_STABLE_ADJACENCY_LIST_DEGREE(v, _alist);
				}
				auto insert_edge(Vert s, Vert t) {
					auto e = _base_type::insert_edge(s, std::move(t));
					GRAPH_V1_STABLE_ADJACENCY_LIST_INSERT_EDGE(std::move(s), e, _alist);
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
				using _alist_type = GRAPH_V1_STABLE_ADJACENCY_LIST_TYPE;
				using In_degree = GRAPH_V1_STABLE_ADJACENCY_LIST_DEGREE_TYPE;
				auto in_edges(const Vert& v) const {
					return GRAPH_V1_STABLE_ADJACENCY_LIST_RANGE(v, _alist);
				}
				auto in_degree(const Vert& v) const {
					return GRAPH_V1_STABLE_ADJACENCY_LIST_DEGREE(v, _alist);
				}
				auto insert_edge(Vert s, Vert t) {
					auto e = _base_type::insert_edge(std::move(s), t);
					GRAPH_V1_STABLE_ADJACENCY_LIST_INSERT_EDGE(std::move(t), e, _alist);
					return e;
				}
			private:
				_alist_type _alist;
			};
		}
	}
}
