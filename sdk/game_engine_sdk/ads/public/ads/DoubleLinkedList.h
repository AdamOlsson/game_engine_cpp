#pragma once
#include <cstddef>
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
};
} // namespace ads
