#pragma once

#include <map>
#include <cassert>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>

#include "Vert_list.hpp"
#include "unordered_set.hpp"
#include "map_iterator_wrapper.hpp"
#include "tracker.hpp"
#include "construct_fn.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class Order_ = std::size_t, class Size_ = std::size_t>
			struct Edge_list :
				Vert_list<Order_> {
				using _base_type = Vert_list<Order_>;
				using Vert = typename _base_type::Vert;
				using Size = Size_;
				using _elist_type = std::map<Size, std::pair<Vert, Vert>>;
				using Edge = map_iterator_wrapper<typename _elist_type::const_iterator, struct edge_tag>;
				auto edges() const {
					return ranges::view::iota(
							_elist.begin(), _elist.end()) |
						ranges::view::transform(construct<Edge>);
				}
				static auto null_edge() noexcept {
					return Edge{};
				}
				auto size() const noexcept {
					return _elist.size();
				}
				static auto tail(const Edge& e) {
					return e._it->second.first;
				}
				static auto head(const Edge& e) {
					return e._it->second.second;
				}
				auto insert_edge(Vert s, Vert t) {
					return Edge{_elist.try_emplace(_elist.end(),
						_elast++, std::move(s), std::move(t))};
				}
				// precondition: vertex must be disconnected
				void erase_vert(const Vert& v) {
#ifndef NDEBUG
					for (auto e : edges())
						assert(tail(e) != v && head(e) != v);
#endif
					_base_type::erase_vert(v);
				}
				void erase_edge(const Edge& e) {
					for (auto& m : _emap_tracker.trackees())
						m._erase(e);
					_elist.erase(e._it);
				}
				void clear() {
					for (auto& m : _emap_tracker.trackees())
						m._clear();
					_elist.clear();
					_elast = 0;
					_base_type::clear();
				}
				template <class T>
				using Edge_map = tracked<persistent_map_iterator_map<Edge, T>, erasable_base<Edge>>;
				template <class T>
				auto edge_map(T default_) const {
					return Edge_map<T>(_emap_tracker, std::move(default_));
				}
				template <class T>
				using Ephemeral_edge_map = ephemeral_map_iterator_map<Edge, T>;
				template <class T>
				auto ephemeral_edge_map(T default_) const {
					return Ephemeral_edge_map<T>(std::move(default_));
				}
				using Edge_set = tracked<erasable_unordered_set<Edge>, erasable_base<Edge>>;
				auto edge_set() const {
					return Edge_set(_emap_tracker);
				}
				using Ephemeral_edge_set = unordered_set<Edge>;
				auto ephemeral_edge_set() const {
					return Ephemeral_edge_set();
				}
			private:
				_elist_type _elist;
				Size _elast = 0;
				tracker<erasable_base<Edge>> _emap_tracker;
			};
		}
	}
}
