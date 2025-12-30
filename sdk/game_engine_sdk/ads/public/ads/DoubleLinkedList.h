#pragma once
#include <cstddef>
#include <iterator>
#include <memory>

namespace ads {
template <typename T> class DoubleLinkedList {
  private:
    struct Node {
        T value;
        std::unique_ptr<Node> next;
        Node *prev;
        Node(T value) : value(std::move(value)), prev(nullptr), next(nullptr) {}
    };
    size_t m_size;
    std::unique_ptr<Node> m_head;
    Node *m_tail;

  public:
    class iterator {
      private:
        Node *m_node;

      public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using iterator_category = std::bidirectional_iterator_tag;

        iterator(Node *node) : m_node(node) {}

        reference operator*() const { return m_node->value; }
        pointer operator->() const { return &m_node->value; }

        iterator &operator++() {
            m_node = m_node->next.get();
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator &operator--() {
            m_node = m_node->prev;
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const iterator &other) const { return m_node == other.m_node; }

        bool operator!=(const iterator &other) const { return m_node != other.m_node; }
    };

    class const_iterator {
      private:
        const Node *m_node;

      public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = const T *;
        using reference = const T &;
        using iterator_category = std::bidirectional_iterator_tag;

        const_iterator(const Node *node) : m_node(node) {}
        const_iterator(const iterator &it) : m_node(it.m_node) {}

        reference operator*() const { return m_node->value; }
        pointer operator->() const { return &m_node->value; }

        const_iterator &operator++() {
            m_node = m_node->next.get();
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        const_iterator &operator--() {
            m_node = m_node->prev;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const const_iterator &other) const {
            return m_node == other.m_node;
        }

        bool operator!=(const const_iterator &other) const {
            return m_node != other.m_node;
        }
    };

    DoubleLinkedList() : m_size(0), m_tail(nullptr) {}

    void push_front(T v) {
        auto new_node = std::make_unique<Node>(std::move(v));
        if (m_size == 0) {
            m_tail = new_node.get();
            m_head = std::move(new_node);
        } else {
            new_node->next = std::move(m_head);
            m_head = std::move(new_node);
            m_head->next->prev = m_head.get();
        }
        m_size++;
    }

    void pop_front() {
        if (m_size == 0) {
            return;
        }
        if (m_size == 1) {
            m_head.reset();
            m_tail = nullptr;
        } else {
            m_head = std::move(m_head->next);
            m_head->prev = nullptr;
        }
        m_size--;
    }

    T &front() const noexcept { return m_head->value; }

    void push_back(T v) {
        auto new_node = std::make_unique<Node>(std::move(v));
        if (m_size == 0) {
            m_tail = new_node.get();
            m_head = std::move(new_node);
        } else {
            new_node->prev = m_tail;
            m_tail->next = std::move(new_node);
            m_tail = m_tail->next.get();
        }
        m_size++;
    }

    void pop_back() {
        if (m_size == 0) {
            return;
        }
        if (m_size == 1) {
            m_head.reset();
            m_tail = nullptr;
        } else {
            m_tail = m_tail->prev;
            m_tail->next.reset();
        }
        m_size--;
    }

    T &back() const { return m_tail->value; }

    size_t size() const noexcept { return m_size; }

    iterator begin() { return iterator(m_head.get()); }
    iterator end() { return iterator(nullptr); }

    const_iterator begin() const { return const_iterator(m_head.get()); }
    const_iterator end() const { return const_iterator(nullptr); }

    const_iterator cbegin() const { return const_iterator(m_head.get()); }
    const_iterator cend() const { return const_iterator(nullptr); }
};
} // namespace ads
