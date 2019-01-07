
#include <catch2/catch.hpp>

#include <graph/Graph.hpp>

#include <random>
#include <sstream>
#include <set>
#include <range/v3/distance.hpp>
#include <range/v3/algorithm/count.hpp>
#include <range/v3/algorithm/sample.hpp>
#include <range/v3/algorithm/permutation.hpp>


template <class G>
struct Graph_tester {
	using V = graph::Vert<G>;
	using E = graph::Edge<G>;
	Graph_tester(G& g) : g(g),
		vm(g.vert_map(g.null_vert())),
		em(g.edge_map(g.null_edge())),
		vs(g.vert_set()),
		es(g.edge_set()) {
		// null objects
		REQUIRE(g.is_null(g.null_vert()));
		REQUIRE(g.is_null(g.null_edge()));
		
		// prepare vert map
		for (auto v : g.verts()) {
			// all verts are unique
			REQUIRE(vm(v) == g.null_vert());
			auto _v = vm.exchange(v, v);
			REQUIRE(_v == g.null_vert());
			vs.insert(v);
		}
		// prepare edge map
		for (auto e : g.edges()) {
			// all edges are unique
			REQUIRE(em(e) == g.null_edge());
			auto _e = em.exchange(e, e);
			REQUIRE(_e == g.null_edge());
			es.insert(e);
		}
		require_invariants();

		// test map reassignment
		for (auto v : g.verts())
			vm.assign(v, v);
		for (auto e : g.edges())
			em.assign(e, e);
		
		// test map exchange
		for (auto v : g.verts()) {
			auto _v = vm.exchange(v, v);
			REQUIRE(_v == v);
		}
		for (auto e : g.edges()) {
			auto _e = em.exchange(e, e);
			REQUIRE(_e == e);
		}

		// test maps on values with destructors
		struct destructible {
			~destructible(){ ++destroyed.get(); }
			std::reference_wrapper<std::size_t> destroyed;
		};
		std::size_t v_destroyed = 0, e_destroyed = 0;
		{
			std::size_t ignore_destroyed = 0;
			auto dvm = g.vert_map(destructible{ignore_destroyed});
			for (auto v : g.verts())
				dvm[v] = destructible{v_destroyed};
			auto dem = g.edge_map(destructible{ignore_destroyed});
			for (auto e : g.edges())
				dem[e] = destructible{e_destroyed};
		}
		REQUIRE(v_destroyed >= g.order());
		REQUIRE(e_destroyed >= g.size());

		// test ephemeral sets and maps here because there isn't a better place without slowing down tests too much
		auto _vm = g.ephemeral_vert_map(g.null_vert());
		auto _vs = g.ephemeral_vert_set();
		for (auto v : g.verts()) {
			_vm[v] = v;
			REQUIRE(!_vs.contains(v));
			REQUIRE(_vs.insert(v));
		}
		for (auto v : g.verts()) {
			REQUIRE(_vm(v) == v);
			REQUIRE(_vs.contains(v));
		}
		REQUIRE(_vs.size() == g.order());
		auto _em = g.ephemeral_edge_map(g.null_edge());
		auto _es = g.ephemeral_edge_set();
		for (auto e : g.edges()) {
			_em[e] = e;
			REQUIRE(!_es.contains(e));
			REQUIRE(_es.insert(e));
		}
		for (auto e : g.edges()) {
			REQUIRE(_em(e) == e);
			REQUIRE(_es.contains(e));
		}
		REQUIRE(_es.size() == g.size());

		// test streaming operator
		std::set<std::string> vnames, enames;
		for (auto v : g.verts()) {
			std::stringstream ss;
			ss << v;
			REQUIRE(vnames.insert(ss.str()).second);
		}
		for (auto e : g.edges()) {
			std::stringstream ss;
			ss << e;
			REQUIRE(enames.insert(ss.str()).second);
		}
	}
	void require_vert_invariants() const {
		for (auto v : g.verts()) {
			REQUIRE(v != g.null_vert());
			REQUIRE(vm(v) == v);
			REQUIRE(vs.contains(v));
		}
		REQUIRE(g.order() == ranges::distance(g.verts()));
	}
	void require_edge_invariants() const {
		for (auto e : g.edges()) {
			REQUIRE(e != g.null_edge());
			REQUIRE(em(e) == e);
			REQUIRE(es.contains(e));
		}
		REQUIRE(g.size() == ranges::distance(g.edges()));
	}
	void require_invariants() const {
		require_vert_invariants();
		require_edge_invariants();
	}
	template <class Random>
	auto random_vert(Random& r) {
		return g.random_vert(r);
	}
	template <class Random>
	auto random_edge(Random& r) {
		return g.random_edge(r);
	}
	template <class Vert_inserter>
	auto test_insert_vert(Vert_inserter vert_inserter) {
		auto m = g.order();
		V v = std::invoke(std::move(vert_inserter), g);
		// new vert is unique and non-null
		REQUIRE(!g.is_null(v));
		REQUIRE(vm(v) == g.null_vert());
		vm[v] = v;
		REQUIRE(vm(v) == v);
		vs.insert(v);
		REQUIRE(vs.contains(v));
		// postconditions
		REQUIRE(ranges::count(g.verts(), v) == 1);
		REQUIRE(g.order() == m + 1);
		require_invariants();
		return v;
	}
	auto insert_vert() {
		auto inserter = [](G& g){ return g.insert_vert(); };
		return test_insert_vert(inserter);
	}
	template <class Edge_inserter>
	auto test_insert_edge(Edge_inserter edge_inserter, V s, V t) {
		auto n = g.size();
		E e = std::invoke(std::move(edge_inserter), g, std::move(s), std::move(t));
		// new edge is unique and non-null
		REQUIRE(!g.is_null(e));
		REQUIRE(em(e) == g.null_edge());
		em[e] = e;
		REQUIRE(em(e) == e);
		es.insert(e);
		REQUIRE(es.contains(e));
		// postconditions
		REQUIRE(ranges::count(g.edges(), e) == 1);
		REQUIRE(g.size() == n + 1);
		REQUIRE(g.tail(e) == s);
		REQUIRE(g.head(e) == t);
		require_invariants();
		return e;
	}
	auto insert_edge(V s, V t) {
		auto inserter = [](G& g, V&& s, V&& t){ return g.insert_edge(std::move(s), std::move(t)); };
		return test_insert_edge(inserter, std::move(s), std::move(t));
	}
	void erase_vert(const V& v) {
		auto m = g.order();
		vm[v] = g.null_vert();
		vs.erase(v);
		g.erase_vert(v);
		// postconditions
		// uncheckable because v was invalidated
		//REQUIRE(ranges::count(g.verts(), v) == 0);
		REQUIRE(g.order() == m - 1);
		require_invariants();
	}
	void erase_edge(const E& e) {
		auto n = g.size();
		em[e] = g.null_edge();
		es.erase(e);
		g.erase_edge(e);
		// postconditions
		// uncheckable because e was invalidated
		//REQUIRE(ranges::count(g.edges(), e) == 0);
		REQUIRE(g.size() == n - 1);
		require_invariants();
	}
	void clear() {
		g.clear();
		// postconditions
		REQUIRE(g.order() == 0);
		REQUIRE(g.size() == 0);
		require_invariants();
	}
protected:
	G& g;
private:
	graph::Vert_map<G, V> vm;
	graph::Edge_map<G, E> em;
	graph::Vert_set<G> vs;
	graph::Edge_set<G> es;
};

template <class G>
struct Out_edge_graph_tester :
	virtual Graph_tester<G> {
	using _base_type = Graph_tester<G>;
	using V = typename _base_type::V;
	using E = typename _base_type::E;
	Out_edge_graph_tester(G& g) :
		_base_type(g),
		om(g.vert_map(g.edge_set())) {
		for (auto v : g.verts()) {
			for (auto e : g.out_edges(v)) {
				om[v].insert(e);
			}
		}
		require_out_edge_invariants();
	}
	void require_out_edge_invariants() const {
		for (auto e : this->g.edges()) {
			const auto& a = om(this->g.tail(e));
			REQUIRE(a.contains(e));
		}
		for (auto v : this->g.verts()) {
			const auto& a = om(v);
			REQUIRE(this->g.out_degree(v) == a.size());
			REQUIRE(ranges::is_permutation(this->g.out_edges(v), a));
		}
	}
	auto insert_vert() {
		auto v = _base_type::insert_vert();
		insert_vert_postconditions(v);
		return v;
	}
	auto insert_vert_postconditions(V v) {
		require_out_edge_invariants();
	}
	auto insert_edge(V s, V t) {
		auto e = _base_type::insert_edge(s, t);
		insert_edge_postconditions(s, t, e);
		return e;
	}
	auto insert_edge_postconditions(V s, V t, E e) {
		REQUIRE(om[s].insert(e));
		require_out_edge_invariants();
	}
	void erase_vert(const V& v) {
		_base_type::erase_vert(v);
		erase_vert_postconditions();
	}
	void erase_vert_postconditions() {
		require_out_edge_invariants();
	}
	void erase_edge(const E& e) {
		erase_edge_preconditions(e);
		_base_type::erase_edge(e);
		erase_edge_postconditions();
	}
	void erase_edge_preconditions(const E& e) {
		REQUIRE(om[this->g.tail(e)].erase(e));
	}
	void erase_edge_postconditions() {
		require_out_edge_invariants();
	}
private:
	graph::Vert_map<G, graph::Edge_set<G>> om;
};

template <class G>
struct In_edge_graph_tester :
	virtual Graph_tester<G> {
	using _base_type = Graph_tester<G>;
	using V = typename _base_type::V;
	using E = typename _base_type::E;
	In_edge_graph_tester(G& g) :
		_base_type(g),
		im(g.vert_map(g.edge_set())) {
		for (auto v : g.verts()) {
			for (auto e : g.in_edges(v)) {
				im[v].insert(e);
			}
		}
		require_in_edge_invariants();
	}
	void require_in_edge_invariants() const {
		for (auto e : this->g.edges()) {
			const auto& a = im(this->g.head(e));
			REQUIRE(a.contains(e));
		}
		for (auto v : this->g.verts()) {
			const auto& a = im(v);
			REQUIRE(this->g.in_degree(v) == a.size());
			REQUIRE(ranges::is_permutation(this->g.in_edges(v), a));
		}
	}
	auto insert_vert() {
		auto v = _base_type::insert_vert();
		insert_vert_postconditions(v);
		return v;
	}
	auto insert_vert_postconditions(V v) {
		require_in_edge_invariants();
	}
	auto insert_edge(V s, V t) {
		auto e = _base_type::insert_edge(s, t);
		insert_edge_postconditions(s, t, e);
		return e;
	}
	auto insert_edge_postconditions(V s, V t, E e) {
		REQUIRE(im[t].insert(e));
		require_in_edge_invariants();
	}
	void erase_vert(const V& v) {
		_base_type::erase_vert(v);
		erase_vert_postconditions();
	}
	void erase_vert_postconditions() {
		require_in_edge_invariants();
	}
	void erase_edge(const E& e) {
		erase_edge_preconditions(e);
		_base_type::erase_edge(e);
		erase_edge_postconditions();
	}
	void erase_edge_preconditions(const E& e) {
		REQUIRE(im[this->g.head(e)].erase(e));
	}
	void erase_edge_postconditions() {
		require_in_edge_invariants();
	}
private:
	graph::Vert_map<G, graph::Edge_set<G>> im;
};

template <class G>
struct Bi_edge_graph_tester :
	Out_edge_graph_tester<G>,
	In_edge_graph_tester<G> {
	using _out_type = Out_edge_graph_tester<G>;
	using _in_type = In_edge_graph_tester<G>;
	using _base_type = Graph_tester<G>;
	using V = typename _base_type::V;
	using E = typename _base_type::E;
	Bi_edge_graph_tester(G& g) :
		_base_type(g),
		_out_type(g),
		_in_type(g) {
	}
	auto insert_vert() {
		auto v = _base_type::insert_vert();
		_out_type::insert_vert_postconditions(v);
		_in_type::insert_vert_postconditions(v);
		return v;
	}
	auto insert_edge(V s, V t) {
		auto e = _base_type::insert_edge(s, t);
		_out_type::insert_edge_postconditions(s, t, e);
		_in_type::insert_edge_postconditions(s, t, e);
		return e;
	}
	void erase_vert(const V& v) {
		_base_type::erase_vert(v);
		_out_type::erase_vert_postconditions();
		_in_type::erase_vert_postconditions();
	}
	void erase_edge(const E& e) {
		_out_type::erase_edge_preconditions(e);
		_in_type::erase_edge_preconditions(e);
		_base_type::erase_edge(e);
		_out_type::erase_edge_postconditions();
		_in_type::erase_edge_postconditions();
	}
};
