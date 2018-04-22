#pragma once

#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>

#include "construct_fn.hpp"
#include "integral_wrapper.hpp"
#include "contiguous_key_map.hpp"
#include "unordered_set.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class Order_ = std::size_t>
			struct Stable_vert_list {
				using Order = Order_;
				using Vert = integral_wrapper<Order, struct vert_tag>;
				auto verts() const noexcept {
					return ranges::view::iota(Order{}, _vlast) |
						ranges::view::transform(construct<Vert>);
				}
				static auto null_vert() noexcept {
					return Vert{};
				}
				auto order() const noexcept {
					return _vlast;
				}
				auto insert_vert() noexcept {
					return Vert{_vlast++};
				}
				template <class T>
				using Vert_map = persistent_contiguous_key_map<Vert, T>;
				template <class T>
				auto vert_map(T default_) const {
					return Vert_map<T>(std::move(default_));
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
				Order _vlast = 0;
			};
		}
	}
}
