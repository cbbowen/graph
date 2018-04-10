#pragma once

#include <unordered_map>
#include <iterator>
#include <tuple>
#include <ostream>

#include "erasable_base.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class It, class = void>
			struct map_iterator_wrapper {
				using key_type = std::decay_t<std::tuple_element_t<0,
					typename std::iterator_traits<It>::value_type>>;
				explicit constexpr map_iterator_wrapper(It it) :
					_it(std::move(it)) {
				}
				constexpr map_iterator_wrapper() : _it{} {}
				// These don't use the key because it can be implemented more efficiently
				bool operator==(const map_iterator_wrapper& other) const {
					return _it == other._it;
				}
				bool operator!=(const map_iterator_wrapper& other) const {
					return _it != other._it;
				}
#define GRAPH_V1_DETAIL_INTEGRAL_WRAPPER_DEFINE_OP(OP) \
				constexpr bool operator OP(const map_iterator_wrapper& other) const \
				{ return key() OP other.key(); }
				//GRAPH_V1_DETAIL_INTEGRAL_WRAPPER_DEFINE_OP(==)
				//GRAPH_V1_DETAIL_INTEGRAL_WRAPPER_DEFINE_OP(!=)
				GRAPH_V1_DETAIL_INTEGRAL_WRAPPER_DEFINE_OP(< )
				GRAPH_V1_DETAIL_INTEGRAL_WRAPPER_DEFINE_OP(> )
				GRAPH_V1_DETAIL_INTEGRAL_WRAPPER_DEFINE_OP(<=)
				GRAPH_V1_DETAIL_INTEGRAL_WRAPPER_DEFINE_OP(>=)
#undef GRAPH_V1_DETAIL_INTEGRAL_WRAPPER_DEFINE_OP
				key_type key() const {
					using std::get;
					return get<0>(*_it);
				}
				template <class Char, class Traits>
				friend decltype(auto) operator<<(std::basic_ostream<Char, Traits>& s, const map_iterator_wrapper& x) {
					return s << x.key();
				}
				It _it;
			};

			template <class It, class Tag, class T>
			struct _map_iterator_map_base {
				// Extending std::hash to map_iterator_wrapper and using an `unordered_map<map_iterator_wrapper, ...>` would look nice, but it would involve an indirection at every hash computation.
				using key_type = map_iterator_wrapper<It, Tag>;
				using inner_key_type = typename key_type::key_type;
				_map_iterator_map_base(T default_) :
					_default(std::move(default_)) {
				}
				const T& operator()(const key_type& k) const {
					if (auto it = _map.find(k.key()); it != _map.end())
						return it->second;
					return _default;
				}
				// TODO: This usually double assigns the value on new keys, which can potentially be corrected by using a reference proxy, but achieving the correct semantics may be difficult.  To avoid this, use `assign` instead.
				T& operator[](const key_type& k) {
					return _map.try_emplace(k.key(), _default).first->second;
				}
				template <class U>
				void assign(const key_type& k, U&& u) {
					_map.insert_or_assign(k, std::forward<U>(u));
					//if (auto [it, inserted] = _map.try_emplace(k, u); !inserted)
					//	return std::exchange(*it, std::forward<U>(u));
					//return _default;
				}
			protected:
				T _default;
				std::unordered_map<inner_key_type, T> _map;
			};

			template <class K, class T>
			struct persistent_map_iterator_map;

			template <class It, class Tag, class T>
			struct persistent_map_iterator_map<map_iterator_wrapper<It, Tag>, T> :
				_map_iterator_map_base<It, Tag, T>,
				erasable_base<map_iterator_wrapper<It, Tag>> {
				using _base_type = _map_iterator_map_base<It, Tag, T>;
				using key_type = typename _base_type::key_type;
				using _base_type::_base_type;
				void _erase(const key_type& k) override {
					this->_map.erase(k.key());
				}
				void _clear() override {
					this->_map.clear();
				}
			};

			template <class K, class T>
			struct ephemeral_map_iterator_map;

			template <class It, class Tag, class T>
			struct ephemeral_map_iterator_map<map_iterator_wrapper<It, Tag>, T> :
				_map_iterator_map_base<It, Tag, T> {
				using _base_type = _map_iterator_map_base<It, Tag, T>;
				using _base_type::_base_type;
			};
		}
	}
}

namespace std {
	template <class It, class Tag>
	struct hash<::graph::v1::impl::map_iterator_wrapper<It, Tag>> {
		using argument_type = ::graph::v1::impl::map_iterator_wrapper<It, Tag>;
		auto operator()(const argument_type& a) const {
			return _inner_hash(a.key());
		}
	private:
		using _inner_hash_type = hash<typename argument_type::key_type>;
		_inner_hash_type _inner_hash;
	};
}
