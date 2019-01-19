#pragma once

#include <vector>

#include "Stable_vert_list.hpp"
#include "construct_fn.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class Order_ = std::size_t, class Size_ = std::size_t>
			struct Stable_edge_list :
				Stable_vert_list<Order_> {
				using _base_type = Stable_vert_list<Order_>;
				using Vert = typename _base_type::Vert;
				using Size = Size_;
				using Edge = integral_wrapper<Size, struct edge_tag>;
				auto edges() const {
					return ranges::view::iota(Size{}, size()) |
						ranges::view::transform(construct<Edge>);
				}
				auto null_edge() const noexcept {
					return Edge{};
				}
				auto size() const noexcept {
					return _elist.size();
				}
				auto tail(const Edge& e) const {
					return _elist[e.key()].first;
				}
				auto head(const Edge& e) const {
					return _elist[e.key()].second;
				}
				auto insert_edge(Vert s, Vert t) {
					auto e = Edge(_elist.size());
					_elist.emplace_back(s, t);
					return e;
				}
				template <class T>
				using Edge_map = persistent_contiguous_key_map<Edge, T>;
				template <class T>
				auto edge_map(T default_) const {
					return Edge_map<T>(size(), std::move(default_));
				}
				template <class T>
				using Ephemeral_edge_map = ephemeral_contiguous_key_map<Edge, T>;
				template <class T>
				auto ephemeral_edge_map(T default_) const {
					return Ephemeral_edge_map<T>(size(), std::move(default_));
				}

				using Edge_set = unordered_set<Edge>;
				auto edge_set() const {
					return Edge_set();
				}
				//using Ephemeral_edge_set = Edge_set;
				using Ephemeral_edge_set = ephemeral_contiguous_key_set<Edge>;
				auto ephemeral_edge_set() const {
					//return edge_set();
					return Ephemeral_edge_set(size());
				}
			private:
				std::vector<std::pair<Vert, Vert>> _elist;
			};
		}
	}
}
