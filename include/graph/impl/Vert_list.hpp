#pragma once

#include <set>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>

#include "construct_fn.hpp"
#include "tracker.hpp"
#include "map_iterator_wrapper.hpp"
#include "unordered_set.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class Order_ = std::size_t>
			struct Vert_list {
				using Order = Order_;
				using _vlist_type = std::set<std::tuple<Order>>;
				using Vert = map_iterator_wrapper<typename _vlist_type::const_iterator, struct vert_tag>;
				auto verts() const noexcept {
					return ranges::view::iota(
							_vlist.begin(), _vlist.end()) |
						ranges::view::transform(construct<Vert>);
				}
				static auto null_vert() noexcept {
					return Vert{};
				}
				auto order() const noexcept {
					return _vlist.size();
				}
				auto insert_vert() {
					return Vert{_vlist.emplace_hint(_vlist.end(), _vlast++)};
				}
				void erase_vert(const Vert& v) {
					for (auto& m : _vmap_tracker.trackees())
						m._erase(v);
					_vlist.erase(v._it);
				}
				void clear() {
					for (auto& m : _vmap_tracker.trackees())
						m._clear();
					_vlist.clear();
					_vlast = 0;
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
			private:
				_vlist_type _vlist;
				Order _vlast = 0;
				tracker<erasable_base<Vert>> _vmap_tracker;
			};
		}
	}
}
