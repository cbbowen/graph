#pragma once

#include <atomic>

#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>

#include "construct_fn.hpp"
#include "tracker.hpp"
#include "exceptions.hpp"
#include "integral_wrapper.hpp"
#include "contiguous_key_map.hpp"
#include "unordered_set.hpp"

#include "Atomic_vert_list.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class Order_ = std::size_t, class Size_ = std::size_t>
			struct Atomic_edge_list :
				Atomic_vert_list<Order_> {
				using _base_type = Atomic_vert_list<Order_>;
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
					return _elast.load();
				}
				auto tail(const Edge& e) const {
					return _elist[e.key()].first;
				}
				auto head(const Edge& e) const {
					return _elist[e.key()].second;
				}
				void reserve_edges(Size capacity) {
					if (capacity > edge_capacity()) {
						_elist.resize(capacity);
						for (auto& m : _emap_tracker.trackees())
							m._reserve(capacity);
					}
				}
				Size edge_capacity() const noexcept {
					return _elist.size();
				}
				auto atomic_insert_edge(Vert s, Vert t) {
					auto ek = _elast++;
					check_precondition(ek < edge_capacity(), "insufficient edge capacity");
					_elist[ek] = std::make_pair(s, t);
					return Edge(ek);
				}
				auto insert_edge(Vert s, Vert t) {
					auto ek = _elast++;
					if (ek < edge_capacity()) {
						_elist[ek] = std::make_pair(s, t);
					} else {
						assert(ek == _elist.size());
						_elist.emplace_back(s, t);
					}
					return Edge(ek);
				}
				using _emap_tracked_type = reservable_base<Edge>;
				using _emap_tracker_type = tracker<_emap_tracked_type, lock_insert_erase_set<_emap_tracked_type *>>;
				template <class T>
				using Edge_map = tracked<persistent_contiguous_key_map<Edge, T>, _emap_tracker_type>;
				template <class T>
				auto edge_map(T default_) const {
					return Edge_map<T>(_emap_tracker, _elist.size(), std::move(default_));
				} // LCOV_EXCL_LINE (unreachable)
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
				using Ephemeral_edge_set = ephemeral_contiguous_key_set<Edge>;
				auto ephemeral_edge_set() const {
					return Ephemeral_edge_set(size());
				}
			private:
				std::vector<std::pair<Vert, Vert>> _elist;
				std::atomic<Size> _elast = 0;
				_emap_tracker_type _emap_tracker;
			};
		}
	}
}
