#pragma once

#include <range/v3/view/transform.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/single.hpp>

#include "traits.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <template <class> class Adjacency, class G>
			class _Subforest {
				using Verts = traits::Verts<G>;
				using Edges = traits::Edges<G>;
			public:
				using Vert = typename Verts::value_type;
				using Order = typename Verts::size_type;
				using Edge = typename Edges::value_type;
				using Size = typename Edges::size_type;
				using Adjacencies = Adjacency<G>;
				_Subforest(const G& g) : _g(g),
					_edges(_g.get().ephemeral_vert_map(_g.get().null_edge())) {
				}
				auto verts() const {
					return Verts::range(_g);
				}
				auto order() const {
					return Verts::size(_g);
				}
				Vert null_vert() const {
					return Verts::null(_g);
				}
				auto edges() const {
					return verts() |
						ranges::view::transform([this](auto v){ return _edges(v); }) |
						ranges::view::remove_if([ne=null_edge()](auto e){ return e == ne; });
				}
				auto size() const {
					return _size;
				}
				Edge null_edge() const {
					return Edges::null(_g);
				}
				auto tail(const Edge& e) const {
					return Edges::tail(_g, e);
				}
				auto head(const Edge& e) const {
					return Edges::head(_g, e);
				}

				// TODO: These map types are suboptimal in some cases because the underlying graph may assume contiguity

				using Vert_set = typename Verts::set_type;
				auto vert_set() const {
					return Verts::set(_g);
				}
				template <class T>
				using Vert_map = typename Verts::template ephemeral_map_type<T>;
				template <class T>
				auto vert_map(T default_) const {
					return Verts::ephemeral_map(std::move(default_));
				}
				template <class T>
				using Ephemeral_vert_map = Vert_map<T>;
				template <class T>
				auto ephemeral_vert_map(T default_) const {
					return vert_map(std::move(default_));
				}

				using Edge_set = typename Edges::set_type;
				auto edge_set() const {
					return Edges::set(_g);
				}
				template <class T>
				using Edge_map = typename Edges::template ephemeral_map_type<T>;
				template <class T>
				auto edge_map(T default_) const {
					return Edges::map(std::move(default_));
				}
				template <class T>
				using Ephemeral_edge_map = Edge_map<T>;
				template <class T>
				auto ephemeral_edge_map(T default_) const {
					return edge_map(std::move(default_));
				}

				void insert_edge(Edge e) {
					// TODO: This should check for cycles in debug builds
					auto k = traits::trait_key<Adjacencies>(_g, e);
					_edges.assign(k, std::move(e));
				}
				void erase_edge(const Edge& e) {
					auto k = traits::trait_key<Adjacencies>(_g, e);
					_edges.assign(k, null_edge());
				}

			protected:
				using _Degree_type = int;
				auto _key_edge_or_null(const Vert& v) const {
					return _edges(v);
				}
				auto _key_edges(const Vert& v) const {
					return ranges::view::single(_edges(v)) |
						ranges::view::remove_if([ne=null_edge()](auto e){ return e == ne; });
				}
				_Degree_type _key_degree(const Vert& v) const {
					return (_edges(v) != null_edge()) ? 1 : 0;
				}
				std::vector<Edge> _key_path(Vert v) const {
					std::vector<Edge> path;
					for (Edge e; (e = _edges(v)) != null_edge(); path.push_back(e))
						v = traits::trait_cokey<Adjacencies>(_g, e);
					return path;
				}

			private:
				std::reference_wrapper<const G> _g;
				typename Verts::template ephemeral_map_type<Edge> _edges;
				typename Edges::size_type _size = 0;
			};
			template <class Adjacency>
			struct Subforest;
			template <class G>
			struct Subforest<traits::Out_edges<G>> :
				_Subforest<traits::Out_edges, G> {
				using _base_type = _Subforest<traits::Out_edges, G>;
				using Vert = typename _base_type::Vert;
				using Edge = typename _base_type::Edge;
				using Out_degree = typename _base_type::_Degree_type;
				using _base_type::_base_type;
				auto out_edge_or_null(const Vert& v) const {
					return _key_edge_or_null(v);
				}
				auto out_edges(const Vert& v) const {
					return this->_key_edges(v);
				}
				Out_degree out_degree(const Vert& v) const {
					return this->_key_degree(v);
				}
				auto path_to_root_from(const Vert& v) const {
					return this->_key_path(v);
				}
			};
			template <class G>
			struct Subforest<traits::In_edges<G>> :
				_Subforest<traits::In_edges, G> {
				using _base_type = _Subforest<traits::In_edges, G>;
				using Vert = typename _base_type::Vert;
				using Edge = typename _base_type::Edge;
				using In_degree = typename _base_type::_Degree_type;
				using _base_type::_base_type;
				auto in_edge_or_null(const Vert& v) const {
					return _key_edge_or_null(v);
				}
				auto in_edges(const Vert& v) const {
					return this->_key_edges(v);
				}
				In_degree in_degree(const Vert& v) const {
					return this->_key_degree(v);
				}
				auto path_from_root_to(const Vert& v) const {
					auto path = this->_key_path(v);
					std::reverse(path.begin(), path.end());
					return path;
				}
			};
			namespace traits {
				// TODO: This should specialize Verts and Edges to avoid typedef repetition above
			}
		}
	}
}
