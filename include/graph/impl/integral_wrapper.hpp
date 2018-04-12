#pragma once

#include <limits>
#include <ostream>

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class I, class = void>
			struct integral_wrapper {
				using key_type = I;
				using flag_type = char; // Why is this `char` instead of `bool`?  `std::vector<bool>`, that's why.
				explicit constexpr integral_wrapper(I i, flag_type flag = static_cast<flag_type>(true)) : _i(i) {
					assert(i != std::numeric_limits<I>::max());
					assert(flag);
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
				key_type key() const {
					return _i;
				}
				flag_type flag() const {
					return static_cast<flag_type>(true);
				}
				template <class Char, class Traits>
				friend decltype(auto) operator<<(std::basic_ostream<Char, Traits>& s, const integral_wrapper& x) {
					return s << x._i;
				}
			private:
				I _i;
			};
		}
	}
}

namespace std {
	template <class I, class Tag>
	struct hash<::graph::v1::impl::integral_wrapper<I, Tag>> {
		using argument_type = ::graph::v1::impl::integral_wrapper<I, Tag>;
		auto operator()(const argument_type& a) const {
			return _inner_hash(a.key());
		}
	private:
		using _inner_hash_type = hash<I>;
		_inner_hash_type _inner_hash;
	};
}
