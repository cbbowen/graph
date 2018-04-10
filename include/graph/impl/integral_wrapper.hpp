#pragma once

#include <utility>
#include <limits>
#include <vector>
#include <ostream>

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class I, class = void>
			struct integral_wrapper {
				explicit constexpr integral_wrapper(I i) : _i(i) {
					assert(i != std::numeric_limits<I>::max());
				}
				constexpr integral_wrapper() :
					_i(std::numeric_limits<I>::max()) {
				}
#define GRAPH_V1_DETAIL_INTEGRAL_WRAPPER_DEFINE_OP(OP) \
				constexpr bool operator OP(const integral_wrapper& other) const \
				{ return _i OP other._i; }
				GRAPH_V1_DETAIL_INTEGRAL_WRAPPER_DEFINE_OP(==)
				GRAPH_V1_DETAIL_INTEGRAL_WRAPPER_DEFINE_OP(!=)
				GRAPH_V1_DETAIL_INTEGRAL_WRAPPER_DEFINE_OP(< )
				GRAPH_V1_DETAIL_INTEGRAL_WRAPPER_DEFINE_OP(> )
				GRAPH_V1_DETAIL_INTEGRAL_WRAPPER_DEFINE_OP(<=)
				GRAPH_V1_DETAIL_INTEGRAL_WRAPPER_DEFINE_OP(>=)
#undef GRAPH_V1_DETAIL_INTEGRAL_WRAPPER_DEFINE_OP
				explicit operator I() const {
					return _i;
				}
				template <class Char, class Traits>
				friend decltype(auto) operator<<(std::basic_ostream<Char, Traits>& s, const integral_wrapper& x) {
					return s << x._i;
				}
			private:
				I _i;
			};
			template <class K, class T>
			struct ephemeral_contiguous_map;
			template <class I, class Tag, class T>
			struct ephemeral_contiguous_map<integral_wrapper<I, Tag>, T> {
				using _container_type = std::vector<T>;
				using int_type = I;
				using key_type = integral_wrapper<I, Tag>;
				using const_reference = typename _container_type::const_reference;
				using reference = typename _container_type::reference;
				ephemeral_contiguous_map(std::size_t size, T default_ = {}) :
					_map(size, std::move(default_)) {
				}
				const_reference operator()(const key_type& k) const {
					auto i = static_cast<int_type>(k);
					return _map[i];
				}
				reference operator[](const key_type& k) {
					auto i = static_cast<int_type>(k);
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
			struct persistent_contiguous_map;
			template <class I, class Tag, class T>
			struct persistent_contiguous_map<integral_wrapper<I, Tag>, T> {
				using _container_type = std::vector<T>;
				using int_type = I;
				using key_type = integral_wrapper<I, Tag>;
				using const_reference = const T&;
				using reference = typename _container_type::reference;
				persistent_contiguous_map(T default_ = {}) :
					_default(std::move(default_)) {
				}
				const_reference operator()(const key_type& k) const {
					auto i = static_cast<int_type>(k);
					if (i < _map.size())
						return _map[i];
					return _default;
				}
				reference operator[](const key_type& k) {
					auto i = static_cast<int_type>(k);
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
			struct ephemeral_contiguous_set;
			template <class I, class Tag>
			struct ephemeral_contiguous_set<integral_wrapper<I, Tag>> {
				using int_type = I;
				using key_type = integral_wrapper<I, Tag>;
				using _container_type = std::vector<key_type>;
				using iterator = typename _container_type::iterator;
				ephemeral_contiguous_set(std::size_t size) :
					_bitmap(size, false) {
				}
				auto size() const {
					return _container.size();
				}
				bool contains(const key_type& k) const {
					return _bitmap(k);
				}
				bool insert(const key_type& k) {
					// If it's already there, nothing else to do
					if (_bitmap.exchange(k, true))
						return false;
					_container.push_back(k);
					return true;
				}
				// TODO: `erase`, `clear`, `begin`, and `end` are experimental
				bool erase(const key_type& k) const {
					// TODO: This implementation is O(n)
					if (_bitmap.exchange(k, false)) {
						_container.erase(std::find(_container.begin(), _container.end(), k));
						return true;
					}
					return false;
				}
				void clear() {
					for (auto k : _container)
						_bitmap[k] = false;
					_container.clear();
				}
				iterator begin() const {
					return _container.begin();
				}
				iterator end() const {
					return _container.end();
				}
			private:
				ephemeral_contiguous_map<key_type, char> _bitmap; // Why is this `char` instead of `bool`?  `std::vector<bool>`, that's why.
				_container_type _container;
			};
		}
	}
}

namespace std {
	template <class I, class Tag>
	struct hash<::graph::v1::impl::integral_wrapper<I, Tag>> {
		using argument_type = ::graph::v1::impl::integral_wrapper<I, Tag>;
		auto operator()(const argument_type& a) const {
			return _inner_hash(static_cast<I>(a));
		}
	private:
		using _inner_hash_type = hash<I>;
		_inner_hash_type _inner_hash;
	};
}
