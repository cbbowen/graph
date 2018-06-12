#pragma once

#include <utility>
#include <type_traits>
#include <vector>
#include <memory>
#include <algorithm>

#include "reservable_base.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class K, class T>
			struct ephemeral_contiguous_key_map {
				// One might reasonably wonder why we don't just use `std::vector<T>` here.  The reason is to make this usable with types that aren't copyable or even movable, like `std::atomic<...>`.  Of course, the user could make a wrapper around `std::atomic<...>` that is movable, but if we can make it unnecessary in this case, why not do so?
				using _container_type = std::unique_ptr<T[]>;
				using key_type = K;
				using value_type = T;
				using const_reference = const value_type&;
				using reference = value_type&;
				using _inner_key_type = typename key_type::key_type;
				static_assert(std::is_integral_v<_inner_key_type>);
				explicit ephemeral_contiguous_key_map(std::size_t size) :
					// Note that the elements will be value-initialized, which is the correct behavior with no default is specified
					_map(std::make_unique<T[]>(size)) {
				}
				ephemeral_contiguous_key_map(std::size_t size, T default_) :
				// Note that the elements will be default-initialized
					_map(new T[size]) {
					// Annoyingly, this approach uses copy-assignment instead of copy-construction, but copy construction isn't possible with `new []`, which we need to use because `std::default_delete` calls `delete []`.
					std::fill_n(_map.get(), size, default_);
				}
				const_reference operator()(const key_type& k) const {
					auto i = k.key();
					return _map[i];
				}
				reference operator[](const key_type& k) {
					auto i = k.key();
					return _map[i];
				}
				template <class U>
				void assign(const key_type& k, U&& u) {
					(*this)[k] = std::forward<U>(u);
				}
				template <class U>
				T exchange(const key_type& k, U&& u) {
					return std::exchange((*this)[k], std::forward<U>(u));
				}
				bool operator==(const ephemeral_contiguous_key_map& other) const {
					// It is undefined behavior to change the underlying graph during the lifetime of *this or other, so we can safely assume these have identical domains.
					return _map == other._map;
				}
				bool operator!=(const ephemeral_contiguous_key_map& other) const {
					return !(*this == other);
				}
			private:
				_container_type _map;
			};
			template <class K, class T>
			struct persistent_contiguous_key_map : reservable_base<K> {
				using _container_type = std::vector<T>;
				using key_type = K;
				using value_type = T;
				using const_reference = const T&;
				using reference = typename _container_type::reference;
				using _inner_key_type = typename key_type::key_type;
				static_assert(std::is_integral_v<_inner_key_type>);
				explicit persistent_contiguous_key_map(T default_ = {}) :
					_default(std::move(default_)) {
				}
				persistent_contiguous_key_map(std::size_t size, T default_) :
					persistent_contiguous_key_map(std::move(default_)) {
					reserve(size);
				}
				void reserve(typename _container_type::size_type capacity) {
					// The motivation for this method is to enable parallel access to independent values, so `_map.reserve(capacity)` isn't sufficient
					_map.resize(std::max(capacity, _map.size()), _default);
				}
				const_reference operator()(const key_type& k) const {
					auto i = k.key();
					if (i < _map.size())
						return _map[i];
					return _default;
				}
				reference operator[](const key_type& k) {
					auto i = k.key();
					if (!(i < _map.size()))
						_map.resize(i + 1, _default);
					return _map[i];
				}
				template <class U>
				void assign(const key_type& k, U&& u) {
					(*this)[k] = std::forward<U>(u);
				}
				template <class U>
				T exchange(const key_type& k, U&& u) {
					return std::exchange((*this)[k], std::forward<U>(u));
				}
				void _reserve(std::size_t capacity) override {
					reserve(capacity);
				}
				// TODO: Figure out how best to implement operator== given that equality should only be over the domain of vertices _currently in the graph_.
			private:
				T _default;
				_container_type _map;
			};
			template <class K>
			struct ephemeral_contiguous_key_set {
				using key_type = K;
				using flag_type = typename key_type::flag_type;
				using _container_type = std::vector<key_type>;
				using size_type = typename _container_type::size_type;
				using iterator = typename _container_type::const_iterator;
				ephemeral_contiguous_key_set(std::size_t size) :
					_flags(size, flag_type{}) {
				}
				auto size() const {
					return _container.size();
				}
				bool contains(const key_type& k) const {
					return static_cast<bool>(_flags(k));
				}
				bool insert(const key_type& k) {
					// If it's already there, nothing else to do
					if (static_cast<bool>(_flags.exchange(k, k.flag())))
						return false;
					_container.push_back(k);
					return true;
				}
				// TODO: `erase`, `clear`, `begin`, and `end` are experimental
				bool erase(const key_type& k) const {
					if (_flags.exchange(k, flag_type{})) {
						// TODO: This implementation is O(n)
						_container.erase(std::find(_container.begin(), _container.end(), k));
						return true;
					}
					return false;
				}
				void clear() {
					for (auto k : _container)
						_flags[k] = flag_type{};
					_container.clear();
				}
				iterator begin() const {
					return _container.begin();
				}
				iterator end() const {
					return _container.end();
				}
				bool operator==(const ephemeral_contiguous_key_set& other) const {
					// It is undefined behavior to change the underlying graph during the lifetime of *this or other, so we can safely assume these have identical domains.
					return _flags == other._flags;
				}
				bool operator!=(const ephemeral_contiguous_key_set& other) const {
					return !(*this == other);
				}
			private:
				ephemeral_contiguous_key_map<key_type, flag_type> _flags;
				_container_type _container;
			};
		}
	}
}
