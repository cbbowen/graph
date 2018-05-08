#pragma once

#include <utility>
#include <iterator>
#include <tuple>
#include <ostream>

#include "erasable_base.hpp"
#include "unordered_key_map.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class It, class = void>
			struct map_iterator_wrapper {
				using key_type = std::decay_t<std::tuple_element_t<0,
					typename std::iterator_traits<It>::value_type>>;
				explicit map_iterator_wrapper(It it) :
					_it(std::move(it)) {
				}
				map_iterator_wrapper() = default;
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

			// Extending std::hash to map_iterator_wrapper and using an `unordered_map<map_iterator_wrapper, ...>` would look nice, but it would involve an indirection at every hash computation.  Instead, we use a map that relies on the key have an inner key that is `EqualityComparable` and `Hashable`.

			template <class K, class T>
			struct persistent_map_iterator_map;

			template <class It, class Tag, class T>
			struct persistent_map_iterator_map<map_iterator_wrapper<It, Tag>, T> :
				unordered_key_map<map_iterator_wrapper<It, Tag>, T>,
				erasable_base<map_iterator_wrapper<It, Tag>> {
				using _base_type = unordered_key_map<map_iterator_wrapper<It, Tag>, T>;
				using key_type = typename _base_type::key_type;
				using _base_type::_base_type;
				void _erase(const key_type& k) override {
					_base_type::_erase(k);
				}
				void _clear() override {
					_base_type::_clear();
				}
			};

			template <class K, class T>
			struct ephemeral_map_iterator_map;

			template <class It, class Tag, class T>
			struct ephemeral_map_iterator_map<map_iterator_wrapper<It, Tag>, T> :
				unordered_key_map<map_iterator_wrapper<It, Tag>, T> {
				using _base_type = unordered_key_map<map_iterator_wrapper<It, Tag>, T>;
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
