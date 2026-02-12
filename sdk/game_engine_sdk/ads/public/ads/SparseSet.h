#pragma once
#include "util/assert.h"
#include <cstddef>
#include <iterator>
#include <limits>
#include <vector>

namespace ads {
template <typename T> class SparseSet {
  private:
    std::vector<T> m_dense;          // Dense array of active elements
    std::vector<size_t> m_sparse;    // ID -> dense index mapping
    std::vector<size_t> m_reverse;   // Dense index -> original ID mapping
    std::vector<size_t> m_available; // Stack of recycled IDs
    size_t m_next_id;                // Next auto-generated ID

    static constexpr size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

  public:
    // Iterator types for dense array iteration
    class iterator {
      private:
        typename std::vector<T>::iterator m_it;
        friend class SparseSet;

      public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using iterator_category = std::random_access_iterator_tag;

        iterator(typename std::vector<T>::iterator it) : m_it(it) {}

        reference operator*() const { return *m_it; }
        pointer operator->() const { return &(*m_it); }

        iterator &operator++() {
            ++m_it;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++m_it;
            return tmp;
        }

        iterator &operator--() {
            --m_it;
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            --m_it;
            return tmp;
        }

        iterator &operator+=(difference_type n) {
            m_it += n;
            return *this;
        }

        iterator operator+(difference_type n) const { return iterator(m_it + n); }

        iterator &operator-=(difference_type n) {
            m_it -= n;
            return *this;
        }

        iterator operator-(difference_type n) const { return iterator(m_it - n); }

        difference_type operator-(const iterator &other) const {
            return m_it - other.m_it;
        }

        reference operator[](difference_type n) const { return m_it[n]; }

        bool operator==(const iterator &other) const { return m_it == other.m_it; }

        bool operator!=(const iterator &other) const { return m_it != other.m_it; }

        bool operator<(const iterator &other) const { return m_it < other.m_it; }

        bool operator<=(const iterator &other) const { return m_it <= other.m_it; }

        bool operator>(const iterator &other) const { return m_it > other.m_it; }

        bool operator>=(const iterator &other) const { return m_it >= other.m_it; }
    };

    class const_iterator {
      private:
        typename std::vector<T>::const_iterator m_it;
        friend class SparseSet;

      public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = const T *;
        using reference = const T &;
        using iterator_category = std::random_access_iterator_tag;

        const_iterator(typename std::vector<T>::const_iterator it) : m_it(it) {}
        const_iterator(const iterator &it) : m_it(it.m_it) {}

        reference operator*() const { return *m_it; }
        pointer operator->() const { return &(*m_it); }

        const_iterator &operator++() {
            ++m_it;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++m_it;
            return tmp;
        }

        const_iterator &operator--() {
            --m_it;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --m_it;
            return tmp;
        }

        const_iterator &operator+=(difference_type n) {
            m_it += n;
            return *this;
        }

        const_iterator operator+(difference_type n) const {
            return const_iterator(m_it + n);
        }

        const_iterator &operator-=(difference_type n) {
            m_it -= n;
            return *this;
        }

        const_iterator operator-(difference_type n) const {
            return const_iterator(m_it - n);
        }

        difference_type operator-(const const_iterator &other) const {
            return m_it - other.m_it;
        }

        reference operator[](difference_type n) const { return m_it[n]; }

        bool operator==(const const_iterator &other) const { return m_it == other.m_it; }

        bool operator!=(const const_iterator &other) const { return m_it != other.m_it; }

        bool operator<(const const_iterator &other) const { return m_it < other.m_it; }

        bool operator<=(const const_iterator &other) const { return m_it <= other.m_it; }

        bool operator>(const const_iterator &other) const { return m_it > other.m_it; }

        bool operator>=(const const_iterator &other) const { return m_it >= other.m_it; }
    };

    // Constructor
    SparseSet() : m_next_id(0) {}

    // Disable copy/move for simplicity (can be added later if needed)
    SparseSet(const SparseSet &) = delete;
    SparseSet &operator=(const SparseSet &) = delete;
    SparseSet(SparseSet &&) = delete;
    SparseSet &operator=(SparseSet &&) = delete;

    // Core operations
    size_t insert(const T &value) {
        size_t id = m_available.empty() ? m_next_id++ : m_available.back();
        if (!m_available.empty()) {
            m_available.pop_back();
        }

        // Ensure sparse array is large enough
        if (id >= m_sparse.size()) {
            m_sparse.resize((id + 1) * 2, INVALID_INDEX);
        }

        // Insert at end of dense array (contiguous storage)
        m_sparse[id] = m_dense.size();
        m_dense.push_back(value);
        m_reverse.push_back(id);

        return id;
    }

    size_t insert(T &&value) {
        size_t id = m_available.empty() ? m_next_id++ : m_available.back();
        if (!m_available.empty()) {
            m_available.pop_back();
        }

        // Ensure sparse array is large enough
        if (id >= m_sparse.size()) {
            m_sparse.resize((id + 1) * 2, INVALID_INDEX);
        }

        // Insert at end of dense array (contiguous storage)
        m_sparse[id] = m_dense.size();
        m_dense.push_back(std::move(value));
        m_reverse.push_back(id);

        return id;
    }

    void remove(size_t id) {
        if (!contains(id)) {
            return;
        }

        size_t dense_index = m_sparse[id];
        size_t last_dense_index = m_dense.size() - 1;
        size_t last_id = m_reverse[last_dense_index];

        // Swap with last element if not already last
        if (dense_index != last_dense_index) {
            m_dense[dense_index] = std::move(m_dense[last_dense_index]);
            m_reverse[dense_index] = last_id;
            m_sparse[last_id] = dense_index;
        }

        // Remove last element and clean up
        m_dense.pop_back();
        m_reverse.pop_back();
        m_sparse[id] = INVALID_INDEX;
        m_available.push_back(id);
    }

    T &get(size_t id) {
        DEBUG_ASSERT(contains(id), "Error: Attempting to get non-existent element.");
        return m_dense[m_sparse[id]];
    }

    const T &get(size_t id) const {
        DEBUG_ASSERT(contains(id), "Error: Attempting to get non-existent element.");
        return m_dense[m_sparse[id]];
    }

    bool contains(size_t id) const {
        return id < m_sparse.size() && m_sparse[id] != INVALID_INDEX;
    }

    size_t size() const { return m_dense.size(); }

    bool empty() const { return m_dense.empty(); }

    void clear() {
        m_dense.clear();
        m_sparse.clear();
        m_reverse.clear();
        m_available.clear();
        m_next_id = 0;
    }

    // Capacity management
    void reserve(size_t capacity) {
        m_dense.reserve(capacity);
        m_reverse.reserve(capacity);

        // Estimate required sparse array capacity based on current usage
        size_t estimated_max_id = m_next_id + capacity;
        if (estimated_max_id > m_sparse.capacity()) {
            m_sparse.reserve(estimated_max_id);
        }
    }

    size_t capacity() const { return m_dense.capacity(); }

    // Iterator support for efficient iteration over active elements
    iterator begin() { return iterator(m_dense.begin()); }

    iterator end() { return iterator(m_dense.end()); }

    const_iterator begin() const { return const_iterator(m_dense.begin()); }

    const_iterator end() const { return const_iterator(m_dense.end()); }

    const_iterator cbegin() const { return const_iterator(m_dense.cbegin()); }

    const_iterator cend() const { return const_iterator(m_dense.cend()); }

    // Direct access to underlying storage (for advanced use cases)
    T &operator[](size_t dense_index) {
        DEBUG_ASSERT(dense_index < m_dense.size(), "Error: Dense index out of bounds.");
        return m_dense[dense_index];
    }

    const T &operator[](size_t dense_index) const {
        DEBUG_ASSERT(dense_index < m_dense.size(), "Error: Dense index out of bounds.");
        return m_dense[dense_index];
    }

    // Get the ID corresponding to a dense index
    size_t get_id(size_t dense_index) const {
        DEBUG_ASSERT(dense_index < m_reverse.size(), "Error: Dense index out of bounds.");
        return m_reverse[dense_index];
    }
};
} // namespace ads
