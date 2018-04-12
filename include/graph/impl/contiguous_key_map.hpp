#pragma once

#include <utility>
#include <type_traits>
#include <vector>

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class K, class T>
			struct ephemeral_contiguous_key_map {
				using _container_type = std::vector<T>;
				using key_type = K;
				using inner_key_type = typename key_type::key_type;
				static_assert(std::is_integral_v<inner_key_type>);
				using const_reference = typename _container_type::const_reference;
				using reference = typename _container_type::reference;
				ephemeral_contiguous_key_map(std::size_t size, T default_ = {}) :
					_map(size, std::move(default_)) {
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
			private:
				_container_type _map;
			};
			template <class K, class T>
			struct persistent_contiguous_key_map {
				using _container_type = std::vector<T>;
				using key_type = K;
				using inner_key_type = typename key_type::key_type;
				static_assert(std::is_integral_v<inner_key_type>);
				using const_reference = const T&;
				using reference = typename _container_type::reference;
				persistent_contiguous_key_map(T default_ = {}) :
					_default(std::move(default_)) {
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
			private:
				T _default;
				_container_type _map;
			};
			template <class K>
			struct ephemeral_contiguous_key_set {
				using key_type = K;
				using flag_type = typename key_type::flag_type;
				using _container_type = std::vector<key_type>;
				using iterator = typename _container_type::iterator;
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
					// TODO: This implementation is O(n)
					if (_flags.exchange(k, flag_type{})) {
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
			private:
				ephemeral_contiguous_key_map<key_type, flag_type> _flags;
				_container_type _container;
			};
		}
	}
}
