
#include <catch.hpp>

#include <graph/Graph.hpp>

#include <random>
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
		// prepare vert map
		for (auto v : g.verts()) {
			// all verts are unique
			REQUIRE(vm(v) == g.null_vert());
			vm[v] = v;
			vs.insert(v);
		}
		// prepare edge map
		for (auto e : g.edges()) {
			// all edges are unique
			REQUIRE(em(e) == g.null_edge());
			em[e] = e;
			es.insert(e);
		}
		require_invariants();
		// test ephemeral maps here because there isn't a better place without slowing down tests too much
		auto _vm = g.ephemeral_vert_map(g.null_vert());
		for (auto v : g.verts())
			_vm[v] = v;
		for (auto v : g.verts())
			REQUIRE(_vm(v) == v);
		auto _em = g.ephemeral_edge_map(g.null_edge());
		for (auto e : g.edges())
			_em[e] = e;
		for (auto e : g.edges())
			REQUIRE(_em(e) == e);
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
	auto insert_vert() {
		auto m = g.order();
		V v = g.insert_vert();
		// new vert is unique
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
	auto insert_edge(V s, V t) {
		auto n = g.size();
		E e = g.insert_edge(s, t);
		// new edge is unique
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
	Graph_tester<G> {
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
		// postconditions
		require_out_edge_invariants();
		return v;
	}
	auto insert_edge(V s, V t) {
		auto e = _base_type::insert_edge(s, t);
		// postconditions
		REQUIRE(om[s].insert(e));
		require_out_edge_invariants();
		return e;
	}
	void erase_vert(const V& v) {
		_base_type::erase_vert(v);
		// postconditions
		require_out_edge_invariants();
	}
	void erase_edge(const E& e) {
		REQUIRE(om[this->g.tail(e)].erase(e));
		_base_type::erase_edge(e);
		// postconditions
		require_out_edge_invariants();
	}
private:
	graph::Vert_map<G, graph::Edge_set<G>> om;
};

template <class G>
struct In_edge_graph_tester :
	Graph_tester<G> {
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
		// postconditions
		require_in_edge_invariants();
		return v;
	}
	auto insert_edge(V s, V t) {
		auto e = _base_type::insert_edge(s, t);
		// postconditions
		REQUIRE(im[t].insert(e));
		require_in_edge_invariants();
		return e;
	}
	void erase_vert(const V& v) {
		_base_type::erase_vert(v);
		// postconditions
		require_in_edge_invariants();
	}
	void erase_edge(const E& e) {
		REQUIRE(im[this->g.head(e)].erase(e));
		_base_type::erase_edge(e);
		// postconditions
		require_in_edge_invariants();
	}
private:
	graph::Vert_map<G, graph::Edge_set<G>> im;
};
