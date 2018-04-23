#pragma once

#include <unordered_set>

#include "erasable_base.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class Key, class Hasher = std::hash<Key>>
			struct unordered_set {
				using _raw_type = std::unordered_set<Key, Hasher>;
				using size_type = typename _raw_type::size_type;
				using key_type = typename _raw_type::key_type;
				using iterator = typename _raw_type::const_iterator;
				auto size() const {
					return _map.size();
				}
				bool contains(const key_type& k) const {
					return _map.find(k) != _map.end();
				}
				bool insert(const key_type& k) {
					return _map.insert(k).second;
				}
				// TODO: `erase`, `clear`, `begin`, and `end` are experimental
				bool erase(const key_type& k) {
					return _map.erase(k);
				}
				void clear() {
					_map.clear();
				}
				iterator begin() const {
					return _map.begin();
				}
				iterator end() const {
					return _map.end();
				}
				_raw_type _raw() && {
					return std::move(_map);
				}
			private:
				_raw_type _map;
			};

			template <class Key, class Hasher = std::hash<Key>>
			struct erasable_unordered_set :
				unordered_set<Key, Hasher>,
				erasable_base<Key> {
				void _erase(const Key& k) override {
					this->erase(k);
				}
				void _clear() override {
					this->clear();
				}
			};
		}
	}
}
