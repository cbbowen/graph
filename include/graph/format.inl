#pragma once

#include <iostream>
#include <map>
#include <stdexcept>

namespace graph {
	inline namespace v1 {
		namespace impl {
			// Allow specification of vertex and edge attributes using a named argument-like syntax:
			// `g.dot_format(vert_attribute_name{"name"} = map, ...)`
			template <class Tag, class Name, class Map> struct attribute_map {
				using tag = Tag;
				attribute_map(Name name, Map&& map) :
					name(name), map(std::forward<Map>(map)) {
				}
				Name name;
				Map map;
			};
			template <class Tag, class Name> struct attribute_name {
				attribute_name(Name name) :
					_name(name) {
				}
				template <class Map> auto operator=(Map& map) const {
					return attribute_map<Tag, Name, Map&>{ _name, map };
				}
				Name _name;
			};

			struct vert_attribute_tag;
			struct edge_attribute_tag;
			template <class Name> using vert_attribute_name = attribute_name<vert_attribute_tag, Name>;
			template <class Name> using edge_attribute_name = attribute_name<edge_attribute_tag, Name>;

			template <class G, class Base = G, class... Args>
			class Dot_format : public Dot_format<G, const Base, Args...> {
				using _base_type = Dot_format<G, const Base, Args...>;
				using _base_type::_base_type;
			public:
				// Note: `const Dot_format&` is not a typo.  This view is not modified, only the underlying graph.
				template <class Char, class Char_traits>
				friend decltype(auto) operator>>(std::basic_istream<Char, Char_traits>& is, const Dot_format& self) {
					static_assert(sizeof...(Args) == 0, "attributes are not yet supported");
					using Verts = traits::Insert_verts<std::reference_wrapper<G>>;
					using Edges = traits::Insert_edges<std::reference_wrapper<G>>;
					self.g.get().clear();
					std::map<std::string, typename Verts::value_type> vm;
					std::string token;
					is >> token;
					if (token != "digraph")
						throw std::runtime_error("expected 'diagraph'");
					std::string name;
					is >> name;
					is >> token;
					if (token != "{")
						throw std::runtime_error("expected '{'");
					auto token_v = [&]() {
						auto it = vm.lower_bound(token);
						if (it->first != token)
							it = vm.try_emplace(it, std::move(token), Verts::insert(self.g));
						return it->second;
					};
					while (is >> token, token != "}") {
						auto v = token_v();
						while (is >> token, token != ";") {
							if (token != "->")
								throw std::runtime_error("expected ';' or '->'");
							is >> token;
							auto u = token_v();
							Edges::insert(self.g, std::move(v), u);
							v = std::move(u);
						}
					}
					return is;
				}
			};
			template <class G, class Base, class... Args>
			class Dot_format<G, const Base, Args...> {
				template <class Tag, class Char, class Char_traits, class K, class Arg>
				static auto output_attribute(std::basic_ostream<Char, Char_traits>& os, const K& k, const Arg& arg, bool any_previous) {
					constexpr auto tag_match = std::is_same<Tag, typename Arg::tag>{};
					if constexpr (tag_match())
						os << (any_previous ? " , " : " [ ") <<
							arg.name << " = " << arg.map(k);
					return tag_match;
				}
				template <class Tag, class Char, class Char_traits, class K, std::size_t... I>
				void output_attributes(std::basic_ostream<Char, Char_traits>& os, const K& k, std::index_sequence<I...>) const {
					// This condition is just an optimization and not require for correctness.
					// TODO: We could further check (at compile time) if any of the argument have a matching tag.
					if constexpr (sizeof...(Args) > 0) {
						bool any = false;
						((any = output_attribute<Tag>(os, k, std::get<I>(args), any)() || any), ...);
						if (any)
							os << " ]";
					}
				}
				template <class Tag, class Char, class Char_traits, class K>
				void output_attributes(std::basic_ostream<Char, Char_traits>& os, const K& k) const {
					output_attributes<Tag>(os, k, std::index_sequence_for<Args...>{});
				}
			protected:
				std::reference_wrapper<G> g;
				std::tuple<Args...> args;
			public:
				Dot_format(G& g, Args&&... args) :
					g(g), args(std::forward<Args>(args)...) {
				}
				template <class Char, class Char_traits>
				friend decltype(auto) operator<<(std::basic_ostream<Char, Char_traits>& os, const Dot_format& self) {
					using Verts = traits::Verts<std::reference_wrapper<G>>;
					using Edges = traits::Edges<std::reference_wrapper<G>>;
					using Order = typename Verts::size_type;
					auto vm = Verts::map(self.g, Order{});
					os << "digraph g {\n";
					Order i = 0;
					for (auto v : Verts::range(self.g)) {
						os << '\t' << (vm[v] = i++);
						self.output_attributes<vert_attribute_tag>(os, v);
						os << " ;\n";
					}
					for (auto e : Edges::range(self.g)) {
						os << '\t' << vm(Edges::tail(self.g, e))
							<< " -> " << vm(Edges::head(self.g, e));
						self.output_attributes<edge_attribute_tag>(os, e);
						os << " ;\n";
					}
					os << "}";
					return os;
				}
			};
			template <class G, class... Args>
			Dot_format(G&, Args&&...) -> Dot_format<G, G, Args...>;
		}
		namespace attributes {
			// Nicer synax for named argument-like attributes:
			// `g.dot_format("name"_of_vert = map, ...)`
			inline auto operator""_of_vert(const char *name, std::size_t) {
				return impl::vert_attribute_name<const char *>{name};
			}
			inline auto operator""_of_edge(const char *name, std::size_t) {
				return impl::edge_attribute_name<const char *>{name};
			}
		}
		template <class Impl>
		template <class... Args>
		auto Graph<Impl>::dot_format(Args&&... args) const {
			return impl::Dot_format(this->_impl(), std::forward<Args>(args)...);
		}
		template <class Impl>
		template <class... Args>
		auto Graph<Impl>::dot_format(Args&&... args) {
			return impl::Dot_format(this->_impl(), std::forward<Args>(args)...);
		}
	}
}
