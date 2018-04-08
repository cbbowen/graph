#pragma once

#include <type_traits>
#include <memory>
#include <unordered_set>
#include <range/v3/view/all.hpp>
#include <range/v3/view/indirect.hpp>

// When disabled, the lifetime of a trackee must not exceed that of its tracker.
#define GRAPH_V1_IMPL_TRACKER_USE_SHARED_PTR 0
#if GRAPH_V1_IMPL_TRACKER_USE_SHARED_PTR
#	define GRAPH_V1_IMPL_TRACKER_IMPL_PTR ::std::shared_ptr
#	define GRAPH_V1_IMPL_TRACKEE_IMPL_PTR ::std::weak_ptr
#	define GRAPH_V1_IMPL_TRACKEE_LOCK_PTR(...) ((__VA_ARGS__).lock())
#else
#	define GRAPH_V1_IMPL_TRACKER_IMPL_PTR ::std::add_pointer_t
#	define GRAPH_V1_IMPL_TRACKEE_IMPL_PTR ::std::add_pointer_t
#	define GRAPH_V1_IMPL_TRACKEE_LOCK_PTR(...) (__VA_ARGS__)
#endif

namespace graph {
	inline namespace v1 {
		namespace impl {
			// Maintains a set of pointers to instances of `T`.
			template <class T>
			struct tracker {
				tracker() :
					_impl(new impl) {
				}
				auto trackees() const {
					return ranges::view::indirect(
						ranges::view::all(*_impl));
				}
				using impl = std::unordered_set<T *>;
				GRAPH_V1_IMPL_TRACKER_IMPL_PTR<impl> _impl;
			};
			// Transform a type into one that can be tracked.
			template <class Derived, class Base = Derived>
			struct tracked final : Derived {
				static_assert(std::is_base_of_v<Base, Derived>);
				using tracker_type = tracker<Base>;
				using tracker_impl = typename tracker_type::impl;
				template <class... Args>
				explicit tracked(const tracker_type& tracker, Args&&... args) :
					tracked(tracker._impl, std::forward<Args>(args)...) {
				}
				tracked(const tracked& other) :
					tracked(other._impl, static_cast<const Derived&>(other)) {
				}
				tracked(tracked&& other) :
					tracked(other._impl, static_cast<Derived&&>(other)) {
				}
				~tracked() {
					if (auto p = GRAPH_V1_IMPL_TRACKEE_LOCK_PTR(_impl))
						p->erase(this);
				}
			private:
				template <class... Args>
				tracked(const _GRAPH_TRACKEE_IMPL_PTR<tracker_impl>& impl, Args&&... args) :
					Derived(std::forward<Args>(args)...),
					_impl(impl) {
					if (auto p = GRAPH_V1_IMPL_TRACKEE_LOCK_PTR(_impl))
						p->insert(this);
				}
				_GRAPH_TRACKEE_IMPL_PTR<tracker_impl> _impl = nullptr;
			};
		}
	}
}

#undef GRAPH_V1_IMPL_TRACKER_IMPL_PTR
#undef GRAPH_V1_IMPL_TRACKEE_IMPL_PTR
#undef GRAPH_V1_IMPL_TRACKEE_LOCK_PTR
#undef GRAPH_V1_IMPL_TRACKER_USE_SHARED_PTR
