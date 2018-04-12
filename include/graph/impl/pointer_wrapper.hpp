#pragma once

#include <functional>

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class T>
			struct pointer_wrapper {
				using key_type = T *;
				using this_type = pointer_wrapper;
				pointer_wrapper(key_type p) noexcept : _p(std::move(p)) {}
				pointer_wrapper(T& r) noexcept : this_type(&r) {}
				pointer_wrapper() noexcept : this_type(nullptr) {}
				key_type key() const { return _p; }
				auto operator ==(const this_type& other) const { return _p == other._p; }
				auto operator !=(const this_type& other) const { return _p != other._p; }
				auto operator <(const this_type& other) const { return _p < other._p; }
				auto operator <=(const this_type& other) const { return _p <= other._p; }
				auto operator >(const this_type& other) const { return _p > other._p; }
				auto operator >=(const this_type& other) const { return _p >= other._p; }
				T& operator*() const { return *_p; }
				T *operator->() const { return _p; }
				template <class Char, class Traits>
				friend decltype(auto) operator<<(std::basic_ostream<Char, Traits>& s, const this_type& x) {
					return s << x.key();
				}
			private:
				friend struct ::std::hash<this_type>;
				key_type _p;
			};
		}
	}
}

namespace std {
	template <class T>
	struct hash<::graph::v1::impl::pointer_wrapper<T>> {
		using argument_type = ::graph::v1::impl::pointer_wrapper<T>;
		auto operator()(const argument_type& a) const {
			return _inner_hash(a.key());
		}
	private:
		using _inner_hash_type = hash<T *>;
		_inner_hash_type _inner_hash;
	};
}
