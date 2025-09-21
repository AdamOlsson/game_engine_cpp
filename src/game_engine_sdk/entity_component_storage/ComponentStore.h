#pragma once
#include <optional>
#include <vector>

template <typename T> class ComponentStoreIterator;

using EntityId = std::size_t;

template <typename T> class ComponentStore {
  private:
    std::vector<T> dense;
    std::vector<EntityId> dense_to_entity;
    std::vector<std::optional<size_t>> sparse;

  public:
    ComponentStore() = default;

    ComponentStore(size_t capacity) {
        dense.reserve(capacity);
        dense_to_entity.reserve(capacity);
        sparse.resize(capacity);
    }

    std::optional<std::reference_wrapper<T>> get(const EntityId id) {
        if (id >= sparse.size() || !sparse[id].has_value()) {
            return std::nullopt;
        }
        return std::ref(dense[sparse[id].value()]);
    }

    std::optional<std::reference_wrapper<std::vector<T>>> get_dense() {
        return std::ref(dense);
    }

    T &add(const EntityId id, T &&component) {
        if (sparse.size() <= id) {
            sparse.resize(std::max(sparse.size() * 2 + 1, id + 1));
        }
        sparse[id] = dense.size();
        dense.push_back(std::move(component));
        dense_to_entity.push_back(id);
        return dense.back();
    }

    template <typename F,
              typename = std::enable_if_t<std::is_invocable_r_v<void, F, T &>>>
    void update(const EntityId id, F &&update_fn) {
        if (id >= sparse.size() || !sparse[id].has_value()) {
            return;
        }
        update_fn(dense[sparse[id].value()]);
    }

    const size_t size() { return dense.size(); };

    ComponentStoreIterator<T> begin() {
        return ComponentStoreIterator<T>(dense_to_entity.data(), dense.data());
    }

    ComponentStoreIterator<T> end() {
        size_t end = dense_to_entity.size() - 1;
        return ComponentStoreIterator<T>(dense_to_entity.data() + dense_to_entity.size(),
                                         dense.data() + dense_to_entity.size());
    }
};

template <typename T> class ComponentStoreIterator {
  private:
    EntityId *current_id;
    T *current;

  public:
    ComponentStoreIterator(EntityId *id_ptr, T *c_ptr)
        : current_id(id_ptr), current(c_ptr) {}

    ComponentStoreIterator &operator++() {
        ++current_id;
        ++current;
        return *this;
    }

    ComponentStoreIterator operator++(int) {
        ComponentStoreIterator temp(current_id, current);
        ++(*this);
        return temp;
    }

    T &operator*() { return *current; }

    bool operator!=(const ComponentStoreIterator &other) const {
        return current_id != other.current_id;
    }

    EntityId id() const { return *current_id; }
    EntityId next() {
        ++(*this);
        return *current_id;
    }
};
