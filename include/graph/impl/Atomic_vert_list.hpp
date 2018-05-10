#pragma once

#include <atomic>
#include <mutex>

#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>

#include "construct_fn.hpp"
#include "tracker.hpp"
#include "exceptions.hpp"
#include "integral_wrapper.hpp"
#include "contiguous_key_map.hpp"
#include "unordered_set.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			// This is only intended to be used with trackers, so the only locked operations are `insert` and `erase`, even iterations is not thread-safe.  Do not treat this as a general thread-safe container.
			template <class T>
			struct lock_insert_erase_set : std::unordered_set<T> {
				using _base_type = std::unordered_set<T>;
				void insert(T value) {
					std::lock_guard lock(_mutex);
					_base_type::insert(std::move(value));
				}
				void erase(const T& value) {
					std::lock_guard lock(_mutex);
					_base_type::erase(value);
				}
				std::mutex _mutex;
			};
			// Guarantees: All const methods and `atomic_*` are atomic unless otherwise noted.
			template <class Order_ = std::size_t>
			struct Atomic_vert_list {
				using Order = Order_;
				using Vert = integral_wrapper<Order, struct vert_tag>;
				auto verts() const {
					return ranges::view::iota(Order{}, order()) |
						ranges::view::transform(construct<Vert>);
				}
				auto null_vert() const noexcept {
					return Vert{};
				}
				auto order() const noexcept {
					return _vlast.load();
				}
				void reserve_verts(Order capacity) {
					if (capacity > vert_capacity()) {
						_vcapacity = capacity;
						for (auto& m : _vmap_tracker.trackees())
							m._reserve(capacity);
					}
				}
				Order vert_capacity() const noexcept {
					return _vcapacity;
				}
				auto atomic_insert_vert() {
					auto vk = _vlast++;
					check_precondition(vk < vert_capacity(), "insufficient vertex capacity");
					return Vert{vk};
				}
				auto insert_vert() {
					auto vk = _vlast++;
					if (vk >= vert_capacity()) {
						assert(_vcapacity == vk);
						++_vcapacity;
					}
					return Vert{vk};
				}
				using _vmap_tracked_type = reservable_base<Vert>;
				using _vmap_tracker_type = tracker<_vmap_tracked_type, lock_insert_erase_set<_vmap_tracked_type *>>;
				template <class T>
				using Vert_map = tracked<persistent_contiguous_key_map<Vert, T>, _vmap_tracker_type>;
				template <class T>
				auto vert_map(T default_) const {
					return Vert_map<T>(_vmap_tracker, _vcapacity, std::move(default_));
				} // LCOV_EXCL_LINE (unreachable)
				template <class T>
				using Ephemeral_vert_map = ephemeral_contiguous_key_map<Vert, T>;
				template <class T>
				auto ephemeral_vert_map(T default_) const {
					return Ephemeral_vert_map<T>(order(), std::move(default_));
				}

				using Vert_set = unordered_set<Vert>;
				auto vert_set() const {
					return Vert_set();
				}
				//using Ephemeral_vert_set = Vert_set;
				using Ephemeral_vert_set = ephemeral_contiguous_key_set<Vert>;
				auto ephemeral_vert_set() const {
					//return vert_set();
					return Ephemeral_vert_set(order());
				}
			private:
				std::atomic<Order> _vlast = 0;
				Order _vcapacity = 0;
				_vmap_tracker_type _vmap_tracker;
			};
		}
	}
}
