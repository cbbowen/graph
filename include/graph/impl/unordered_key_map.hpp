#pragma once

#include <utility>
#include <unordered_map>

#include "erasable_base.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class K, class T>
			struct unordered_key_map {
				using key_type = K;
				using value_type = T;
				using const_reference = const value_type&;
				using reference = value_type&;
				using inner_key_type = typename key_type::key_type;
				using _container_type = std::unordered_map<inner_key_type, T>;
				unordered_key_map(T default_) :
					_default(std::move(default_)) {
				}
				const_reference operator()(const key_type& k) const {
					if (auto it = _map.find(k.key()); it != _map.end())
						return it->second;
					return _default;
				}
				// TODO: This usually double assigns the value on new keys, which can potentially be corrected by using a reference proxy, but achieving the correct semantics may be difficult.  To avoid this, use `assign` instead.
				reference operator[](const key_type& k) {
					return _map.try_emplace(k.key(), _default).first->second;
				}
				template <class U>
				void assign(const key_type& k, U&& u) {
					// This would be better, but is not yet in the MSVC standard library
					//_map.insert_or_assign(k, std::forward<U>(u));
					if (auto [it, inserted] = _map.emplace(k.key(), u); !inserted)
						it->second = std::forward<U>(u);
				}
				template <class U>
				T exchange(const key_type& k, U&& u) {
					if (auto [it, inserted] = _map.emplace(k.key(), u); !inserted)
						return std::exchange(it->second, std::forward<U>(u));
					return _default;
				}
				void _erase(const key_type& k) {
					_map.erase(k.key());
				}
				void _clear() {
					_map.clear();
				}
				// TODO: Figure out how best to implement operator== given that equality should only be over the domain of vertices _currently in the graph_.
			protected:
				T _default;
				_container_type _map;
			};
		}
	}
}
