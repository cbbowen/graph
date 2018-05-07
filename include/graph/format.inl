#pragma once

#include <iostream>
#include <sstream>
#include <cctype>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <type_traits>

namespace graph {
	inline namespace v1 {
		struct format_error : std::runtime_error {
			using _base_type = std::runtime_error;
			format_error(std::initializer_list<const char *> expected, std::string found) :
				_base_type(std::string("expected ") + *expected.begin() + ", found: " + found)
			{}
		};
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
				// TODO: read_attributes(...)
				template <class Char, class Char_traits, class F>
				static bool read_loop(std::basic_istream<Char, Char_traits>& is, F f) {
					for (char c; is.get(c);) {
						if (!f(c)) {
							is.putback(c);
							return true;
						}
					}
					return false;
				}
				template <class Char, class Char_traits>
				static bool skip_whitepsace(std::basic_istream<Char, Char_traits>& is) {
					return read_loop(is, [](char c){ return std::isspace(c); });
				}
				template <class Char, class Char_traits>
				static std::pair<bool, std::basic_string<Char, Char_traits>> read_token(std::basic_istream<Char, Char_traits>& is) {
					skip_whitepsace(is);
					std::basic_string<Char, Char_traits> result;
					bool quoted = false, symbol = false;
					read_loop(is, [&, start=true, escaped=false](char c) mutable {
						// Check if this could start a symbol
						bool is_symbol_start = _base_type::is_symbol_start_char(c);
						if (start) {
							start = false;
							if (c == '"') {
								quoted = true;
								return true;
							}
							if (is_symbol_start)
								symbol = true;
						}
						if (quoted) {
							// If we're in an escape sequence, ignore any special handling
							if (escaped) {
								escaped = false;
							} else {
								// Stop upon reaching an end quote
								if (c == '"') {
									return false;
								}
								// Upon reaching an escape sequence, flag it and return without appending
								if (c == '\\') {
									escaped = true;
									return true;
								}
							}
						} else if (symbol) {
							if (!result.empty()) {
								if (result != "-" || c != '>')
									return false;
							}
						} else {
							// Unless quoted, stop upon reaching whitespace or any reserved symbol
							if (std::isspace(c) || is_symbol_start)
								return false;
						}
						result.push_back(c);
						return true;
					});
					// Remove whitespace from the end unless it was quoted in which case, we need to remove the end quote from the stream
					if (quoted) {
						if (is.get() != '"')
							throw std::runtime_error("expected '\"'");
					} else {
						while (!result.empty() && std::isspace(result.back()))
							result.pop_back();
					}
					return std::make_pair(symbol, result);
				}
				template <class Char, class Char_traits>
				static auto read_identifier(std::basic_istream<Char, Char_traits>& is) {
					auto [symbol, token] = read_token(is);
					if (symbol)
						throw format_error({"identifier"}, token);
					return token;
				}
				template <class Char, class Char_traits>
				static auto read_symbol(std::basic_istream<Char, Char_traits>& is, std::initializer_list<const char *> expected) {
					auto [symbol, token] = read_token(is);
					if (!symbol || std::find_if(expected.begin(), expected.end(), [t=token](const char *e) { return e == t; }) == expected.begin())
						throw format_error(expected, token);
					return token;
				}
				template <class Char, class Char_traits>
				static auto read_type(std::basic_istream<Char, Char_traits>& is) {
					auto [_, token] = read_token(is);
					return token;
				}
				template <class Tag, class K, class Char, class Char_traits, class Arg>
				static bool read_attribute(const K& k, const std::basic_string<Char, Char_traits>& name, const std::basic_string<Char, Char_traits>& value, Arg& arg) {
					if constexpr (std::is_same_v<Tag, typename Arg::tag>) {
						if (arg.name == name) {
							std::istringstream(value) >> arg.map[k];
							return true;
						}
					}
					return false;
				}
				template <class Tag, class K, class Char, class Char_traits, std::size_t... I>
				bool read_attributes(const K& k, const std::basic_string<Char, Char_traits>& name, const std::basic_string<Char, Char_traits>& value, std::index_sequence<I...>) const {
					bool any = false;
					((any = read_attribute<Tag>(k, name, value, std::get<I>(this->args))) ||... || false);
					return any;
				}
				template <class Tag, class K, class Char, class Char_traits>
				bool read_attributes(const K& k, const std::basic_string<Char, Char_traits>& name, const std::basic_string<Char, Char_traits>& value) const {
					return read_attributes<Tag>(k, name, value, std::index_sequence_for<Args...>{});
				}
			public:
				// Note: `const Dot_format&` is not a typo.  This view is not modified, only the underlying graph.
				template <class Char, class Char_traits>
				friend decltype(auto) operator>>(std::basic_istream<Char, Char_traits>& is, const Dot_format& self) {
					using Verts = traits::Insert_verts<G>;
					using Edges = traits::Insert_edges<G>;
					//self._g.get().clear();
					std::map<std::string, typename Verts::value_type> vm;
					if (auto type = read_type(is); type != "digraph")
						throw format_error({"'diagraph'"}, type);
					std::string name = read_identifier(is);
					read_symbol(is, {"'{'"});
					auto get_vertex = [&](const std::string& name) {
						auto it = vm.lower_bound(name);
						if (it == vm.end() || it->first != name)
							it = vm.try_emplace(it, name, Verts::insert(self._g));
						return it->second;
					};
					{
						bool symbol;
						std::string token;
						while (std::tie(symbol, token) = read_token(is), !symbol) {
							auto v = get_vertex(token);
							auto e = Edges::null(self._g);
							for (std::string symbol; (symbol = read_symbol(is, {"'['", "';'", "'->'"})) != ";";) {
								if (symbol == "->") {
									auto u = get_vertex(read_identifier(is));
									e = Edges::insert(self._g, std::move(v), u);
									v = std::move(u);
								} else if (symbol == "[") {
									bool asymbol;
									std::string atoken;
									std::tie(asymbol, atoken) = read_token(is);
									if (!asymbol) {
										while (true) {
											auto aname = atoken;
											read_symbol(is, {"'='"});
											auto value = read_identifier(is);
											if (e == Edges::null(self._g))
												self.read_attributes<vert_attribute_tag>(v, aname, value);
											else
												self.read_attributes<edge_attribute_tag>(e, aname, value);
											atoken = read_symbol(is, {"','", "']'"});
											if (atoken != ",")
												break;
											atoken = read_identifier(is);
										}
										if (atoken != "]")
											throw format_error({"','", "']'"}, atoken);
									} else {
										if (atoken != "]")
											throw format_error({"']'"}, atoken);
									}
								}
							}
						}
						if (token != "}")
							throw format_error({"'}'"}, token);
					}
					return is;
				}
			};
			template <class G, class Base, class... Args>
			class Dot_format<G, const Base, Args...> {
				template <class Char, class Char_traits, class T>
				static decltype(auto) write_attribute_part(std::basic_ostream<Char, Char_traits>& os, const T& part) {
					//return os << part;
					auto ss = std::basic_ostringstream<Char, Char_traits>();
					auto s = std::move(ss.str());
					if (s.empty() || s[0] == '"' || std::find_if(s.begin(), s.end(), [](char c) { return std::isspace(c) || is_symbol_start_char(c); }) != s.end()) {
						using string_t = std::basic_string<Char, Char_traits>;
						auto replace_all = [&s](const string_t& what, const string_t& with) {
							for (typename string_t::size_type i = 0; (i = s.find(what, i)) != s.npos; i += with.size())
								s.replace(i, what.size(), with);
						};
						replace_all("\\", R"(\\)");
						replace_all("\"", R"(\")");
						s = "\"" + s + "\"";
					}
					return os << s;
				}
				template <class Tag, class Char, class Char_traits, class K, class Arg>
				static auto write_attribute(std::basic_ostream<Char, Char_traits>& os, const K& k, const Arg& arg, bool any_previous) {
					constexpr auto tag_match = std::is_same<Tag, typename Arg::tag>{};
					if constexpr (tag_match()) {
						os << (any_previous ? ", " : " [");
						write_attribute_part(os, arg.name) << "=";
						write_attribute_part(os, arg.map(k));
					}
					return tag_match;
				}
				template <class Tag, class Char, class Char_traits, class K, std::size_t... I>
				void write_attributes(std::basic_ostream<Char, Char_traits>& os, const K& k, std::index_sequence<I...>) const {
					// This condition is just an optimization and not require for correctness.
					if constexpr (std::disjunction_v<std::is_same<Tag, typename Args::tag>...>) {
						bool any = false;
						((any = write_attribute<Tag>(os, k, std::get<I>(args), any)() || any), ...);
						if (any)
							os << " ]";
					}
				}
				template <class Tag, class Char, class Char_traits, class K>
				void write_attributes(std::basic_ostream<Char, Char_traits>& os, const K& k) const {
					write_attributes<Tag>(os, k, std::index_sequence_for<Args...>{});
				}
			protected:
				std::reference_wrapper<G> _g;
				std::tuple<Args...> args;
				static bool is_symbol_start_char(char c) {
					return c == '{' || c == '}' || c == '[' || c == ']' || c == '-' || c == ';' || c == '=' || c == ',';
				}
			public:
				Dot_format(G& g, Args&&... args) :
					_g(g), args(std::forward<Args>(args)...) {
				}
				template <class Char, class Char_traits>
				friend decltype(auto) operator<<(std::basic_ostream<Char, Char_traits>& os, const Dot_format& self) {
					using Verts = traits::Verts<std::reference_wrapper<G>>;
					using Edges = traits::Edges<std::reference_wrapper<G>>;
					using Order = typename Verts::size_type;
					auto vm = Verts::map(self._g, Order{});
					os << "digraph g {\n";
					Order i = 0;
					for (auto v : Verts::range(self._g)) {
						os << '\t' << (vm[v] = i++);
						self.write_attributes<vert_attribute_tag>(os, v);
						os << ";\n";
					}
					for (auto e : Edges::range(self._g)) {
						os << '\t' << vm(Edges::tail(self._g, e))
							<< " -> " << vm(Edges::head(self._g, e));
						self.write_attributes<edge_attribute_tag>(os, e);
						os << ";\n";
					}
					os << "}";
					return os;
				}
			};
			template <class G, class... Args>
			Dot_format(G&, Args&&...) -> Dot_format<G, G, Args...>;
		}

		// `g.dot_format(vert_attribute("name") = map, ...)`
		template <class Name> auto vert_attribute(Name name) {
			return impl::vert_attribute_name<Name>{std::move(name)};
		}
		template <class Name> auto edge_attribute(Name name) {
			return impl::edge_attribute_name<Name>{std::move(name)};
		}

		// `g.dot_format(vert_attribute("name", map), ...)`
		template <class Name, class Map> auto vert_attribute(Name name, Map& map) {
			return vert_attribute(std::move(name)) = map;
		}
		template <class Name, class Map> auto edge_attribute(Name name, Map& map) {
			return edge_attribute(std::move(name)) = map;
		}

		namespace attributes {
			// Nicer synax for named argument-like attributes:
			// `g.dot_format("name"_of_vert = map, ...)`
			inline auto operator""_of_vert(const char *name, std::size_t) {
				return vert_attribute(name);
			}
			inline auto operator""_of_edge(const char *name, std::size_t) {
				return edge_attribute(name);
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
