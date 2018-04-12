#pragma once

#include <atomic>
#include <utility>
#include <iterator>

#include <range/v3/iterator_range.hpp>

namespace graph {
	inline namespace v1 {
		namespace impl {
			template <class T>
			struct atomic_list_node {
				template <class... Args>
				atomic_list_node(Args&&... args) :
					_value(std::forward<Args>(args)...) {
				}
				T _value;
				atomic_list_node *_next;
			};
			template <class T, class Node>
			struct atomic_list_iterator {
				using value_type = T;
				using reference = value_type&;
				using pointer = value_type *;
				using iterator_category = std::forward_iterator_tag;
				using difference_type = int;
				atomic_list_iterator(Node *node) : _node(node) {}
				template <class U>
				atomic_list_iterator(const atomic_list_iterator<U, Node>& other) :
					atomic_list_iterator(other._node) {}
				atomic_list_iterator() = default;
				atomic_list_iterator(const atomic_list_iterator&) = default;
				bool operator==(const atomic_list_iterator& other) const { return _node == other._node; }
				bool operator!=(const atomic_list_iterator& other) const { return !(*this == other); }
				reference operator*() const { return _node->_value; }
				pointer operator->() const { return &(**this); }
				atomic_list_iterator& operator++() {
					_node = _node->_next;
					return *this;
				}
				atomic_list_iterator operator++(int) {
					auto result = *this;
					++*this;
					return result;
				}
				Node *_node;
			};
			template <class T>
			struct atomic_list {
				using _node_type = atomic_list_node<T>;
				using value_type = T;
				using const_iterator = atomic_list_iterator<const T, _node_type>;
				using iterator = atomic_list_iterator<T, _node_type>;
				using size_type = unsigned int;
				atomic_list(atomic_list&&) = delete;
				atomic_list(const atomic_list&) = delete;
				atomic_list() = default;
				~atomic_list() {
					_node_type *head = _head.load(std::memory_order_relaxed);
					while (head)
						delete std::exchange(head, head->_next);
				}
				template <class... Args>
				T& emplace(Args&&... args) {
					auto node = new _node_type(std::forward<Args>(args)...);
					node->_next = _head.load(std::memory_order_relaxed);
					// No exceptions possible, so node will never leak
					while (!_head.compare_exchange_weak(node->_next, node, std::memory_order_release, std::memory_order_relaxed))
						;
					++_size;
					// Would this also be correct?
					// _size.fetch_add(1, std::memory_order_release);
					return node->_value;
				} // LCOV_EXCL_LINE (unreachable)
				const_iterator begin() const {
					return iterator{_head.load(std::memory_order_relaxed)};
				}
				const_iterator end() const {
					return iterator{nullptr};
				}
				iterator begin() {
					return iterator{_head.load(std::memory_order_relaxed)};
				}
				iterator end() {
					return iterator{nullptr};
				}
				// This size may be (briefly) be lower than the actual size
				size_type conservative_size() const {
					return _size;
				}
			private:
				std::atomic<_node_type *> _head{nullptr};
				std::atomic<size_type> _size{0};
			};
		}
	}
}
