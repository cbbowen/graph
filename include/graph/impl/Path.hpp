#pragma once

#include <vector>
#include <functional>
#include <range/v3/view/all.hpp>
#include <range/v3/to_container.hpp>

#include "traits.hpp"
#include "exceptions.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class G>
			struct Path {
				using Verts = traits::Verts<G>;
				using Edges = traits::Edges<G>;
				using Vert = typename Verts::value_type;
				using Edge = typename Edges::value_type;
				using _container_type = std::vector<Edge>;
				Path(const G& g, Vert source) :
					_vert(std::move(source)) {
				}
				explicit Path(const G& g) : Path(g, Verts::null(g)) {}
				Path(const G& g, Vert source, _container_type edges) :
					_edges(std::move(edges)),
					_vert(std::move(source)) {
#if GRAPH_CHECK_PRECONDITIONS
					auto v = _vert;
					for (const auto& e : _edges) {
						check_precondition(Edges::tail(g, e) == v, "edges must form a path");
						v = Edges::head(g, e);
					}
#endif
				}
				template <class Edges>
				Path(const G& g, Vert source, Edges&& edges) :
					Path(std::move(source),
						ranges::view::all(std::forward<Edges>(edges)) |
							ranges::to_<_container_type>) {
				}
				auto edges() const {
					return ranges::view::all(_edges);
				}
				bool is_trivial_or_null() const {
					return _edges.empty();
				}
				template <class Weight, class Combine = std::plus<>,
					class D = std::decay_t<std::result_of_t<const Weight&(Edge)>>>
				D total(const Weight& weight, const Combine& combine = {}, const D& zero = {}) const {
					auto result = zero;
					for (const auto& e : _edges)
						result = combine(result, weight(e));
					return result;
				}
				Vert _source(const G& g) const {
					return _vert;
				}
				Vert _target(const G& g) const {
					return _edges.empty() ? _vert : Edges::head(g, _edges.back());
				}
				void _concatenate(const G& g, const Path& other) {
					check_precondition(_target(g) == other._source(g), "paths must be concatenable");
					_edges.insert(_edges.end(), other._edges.begin(), other._edges.end());
				}
			private:
				_container_type _edges;
				Vert _vert;
			};
		}
	}
}
