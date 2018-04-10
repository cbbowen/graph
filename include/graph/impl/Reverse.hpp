#pragma once

#include "traits.hpp"

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class Impl>
			struct Reverse {
				Reverse(const Reverse&) = default;
				Reverse(Reverse&&) = default;
				explicit Reverse(Impl&& impl) :
					_impl(std::forward<Impl>(impl)) {
				}
				Impl _impl;
			};
			namespace traits {
				template <class G>
				struct Verts<Reverse<G>> : Verts<G> {
					using R = Reverse<G>;
					using _base_type = Verts<G>;
					static decltype(auto) range(const R& r) {
						return _base_type::range(r._impl);
					}
					static decltype(auto) size(const R& r) {
						return _base_type::size(r._impl);
					}
					static decltype(auto) null(const R& r) {
						return _base_type::null(r._impl);
					}
					static decltype(auto) set(const R& r) {
						return _base_type::set(r._impl);
					}
					static decltype(auto) ephemeral_set(const R& r) {
						return _base_type::ephemeral_set(r._impl);
					}
					template <class T>
					static decltype(auto) map(const R& r, T default_) {
						return _base_type::map(r._impl, std::move(default_));
					} // LCOV_EXCL_LINE (unreachable)
					template <class T>
					static decltype(auto) ephemeral_map(const R& r, T default_) {
						return _base_type::ephemeral_map(r._impl, std::move(default_));
					}
				};
				template <class G>
				struct Edges<Reverse<G>> : Edges<G> {
					using R = Reverse<G>;
					using _base_type = Edges<G>;
					using value_type = typename _base_type::value_type;
					static decltype(auto) range(const R& r) {
						return _base_type::range(r._impl);
					}
					static decltype(auto) size(const R& r) {
						return _base_type::size(r._impl);
					}
					static decltype(auto) null(const R& r) {
						return _base_type::null(r._impl);
					}
					static decltype(auto) head(const R& r, const value_type& e) {
						return _base_type::tail(r._impl, e);
					}
					static decltype(auto) tail(const R& r, const value_type& e) {
						return _base_type::head(r._impl, e);
					}
					static decltype(auto) set(const R& r) {
						return _base_type::set(r._impl);
					}
					static decltype(auto) ephemeral_set(const R& r) {
						return _base_type::ephemeral_set(r._impl);
					}
					template <class T>
					static decltype(auto) map(const R& r, T default_) {
						return _base_type::map(r._impl, std::move(default_));
					} // LCOV_EXCL_LINE (unreachable)
					template <class T>
					static decltype(auto) ephemeral_map(const R& r, T default_) {
						return _base_type::ephemeral_map(r._impl, std::move(default_));
					}
				};
				template <class G>
				struct Out_edges<Reverse<G>> : In_edges<G> {
					using R = Reverse<G>;
					using _base_type = In_edges<G>;
					using key_type = typename _base_type::key_type;
					static decltype(auto) range(const R& r, const key_type& v) {
						return _base_type::range(r._impl, v);
					}
					static decltype(auto) size(const R& r, const key_type& v) {
						return _base_type::size(r._impl, v);
					}
				};
				template <class G>
				struct In_edges<Reverse<G>> : Out_edges<G> {
					using R = Reverse<G>;
					using _base_type = Out_edges<G>;
					using key_type = typename _base_type::key_type;
					static decltype(auto) range(const R& r, const key_type& v) {
						return _base_type::range(r._impl, v);
					}
					static decltype(auto) size(const R& r, const key_type& v) {
						return _base_type::size(r._impl, v);
					}
				};
			}
		}
	}
}
