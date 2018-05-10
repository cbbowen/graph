#pragma once

#include <type_traits>
#include <memory>
#include <unordered_set>
#include <range/v3/view/all.hpp>
#include <range/v3/view/indirect.hpp>

// When disabled, the lifetime of a trackee must not exceed that of its tracker.
#define GRAPH_V1_IMPL_TRACKER_USE_SHARED_PTR 0
#if GRAPH_V1_IMPL_TRACKER_USE_SHARED_PTR
#	define GRAPH_V1_IMPL_TRACKER_PTR ::std::shared_ptr
#	define GRAPH_V1_IMPL_TRACKEE_PTR ::std::weak_ptr
#	define GRAPH_V1_IMPL_TRACKEE_UNLOCK_PTR(...) (__VA_ARGS__)
#	define GRAPH_V1_IMPL_TRACKEE_LOCK_PTR(...) ((__VA_ARGS__).lock())
#else
#	define GRAPH_V1_IMPL_TRACKER_PTR ::std::unique_ptr
#	define GRAPH_V1_IMPL_TRACKEE_PTR ::std::add_pointer_t
#	define GRAPH_V1_IMPL_TRACKEE_UNLOCK_PTR(...) ((__VA_ARGS__).get())
#	define GRAPH_V1_IMPL_TRACKEE_LOCK_PTR(...) (__VA_ARGS__)
#endif

namespace graph {
	inline namespace v1 {
		namespace impl {
			// Maintains a set of pointers to instances of `T`.
			template <class T, class Set_ = std::unordered_set<T *>>
			struct tracker {
				using set_type = Set_;
				tracker() :
					_set(new set_type()) {
				}
				auto trackees() const {
					return ranges::view::indirect(
						ranges::view::all(*_set));
				}
				GRAPH_V1_IMPL_TRACKER_PTR<set_type> _set;
			};
			// Transform a type into one that can be tracked.
			template <class Derived, class Tracker = tracker<Derived>>
			struct tracked final : Derived {
				using tracker_type = Tracker;
				using tracker_set = typename tracker_type::set_type;
				template <class... Args>
				explicit tracked(const tracker_type& tracker, Args&&... args) :
					tracked(
						GRAPH_V1_IMPL_TRACKEE_UNLOCK_PTR(tracker._set),
						std::forward<Args>(args)...) {
				}
				tracked(const tracked& other) :
					tracked(other._set, static_cast<const Derived&>(other)) {
				}
				tracked(tracked&& other) :
					tracked(other._set, static_cast<Derived&&>(other)) {
				}
				~tracked() {
					if (auto p = GRAPH_V1_IMPL_TRACKEE_LOCK_PTR(_set))
						p->erase(this);
				}
				Derived _untracked() && {
					return Derived(std::move(*this));
				}
			private:
				template <class... Args>
				tracked(const GRAPH_V1_IMPL_TRACKEE_PTR<tracker_set>& set, Args&&... args) :
					Derived(std::forward<Args>(args)...),
					_set(set) {
					if (auto p = GRAPH_V1_IMPL_TRACKEE_LOCK_PTR(_set))
						p->insert(this);
				}
				GRAPH_V1_IMPL_TRACKEE_PTR<tracker_set> _set = nullptr;
			};
		}
	}
}

#undef GRAPH_V1_IMPL_TRACKER_PTR
#undef GRAPH_V1_IMPL_TRACKEE_PTR
#undef GRAPH_V1_IMPL_TRACKEE_LOCK_PTR
#undef GRAPH_V1_IMPL_TRACKER_USE_SHARED_PTR
