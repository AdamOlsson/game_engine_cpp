#pragma once
#include "game_engine_sdk/entity_component_storage/ComponentStore.h"
#include "game_engine_sdk/physics_engine/RigidBody.h"
#include "game_engine_sdk/render_engine/RenderBody.h"
#include <functional>
#include <optional>
#include <type_traits>
#include <utility>

template <typename T> struct is_valid_component : std::false_type {};
template <> struct is_valid_component<RigidBody> : std::true_type {};
template <> struct is_valid_component<RenderBody> : std::true_type {};

template <typename T>
inline constexpr bool is_valid_component_v = is_valid_component<T>::value;

class EntityComponentStorage {
  private:
    ComponentStore<RigidBody> rigid_bodies;
    ComponentStore<RenderBody> render_bodies;
    EntityId next_id = 0;

    template <typename C, typename = std::enable_if_t<is_valid_component_v<C>>>
    constexpr auto &get_store() {
        if constexpr (std::is_same_v<C, RigidBody>) {
            return rigid_bodies;
        } else if constexpr (std::is_same_v<C, RenderBody>) {
            return render_bodies;
        }
    }

  public:
    EntityComponentStorage() = default;
    ~EntityComponentStorage() {}

    EntityId create_entity();

    template <typename C, typename = std::enable_if_t<is_valid_component_v<C>>>
    std::optional<std::reference_wrapper<C>> get_component(const EntityId id) {
        return get_store<C>().get(id);
    }

    template <typename C, typename = std::enable_if_t<is_valid_component_v<C>>>
    std::optional<std::reference_wrapper<std::vector<C>>> get_component() {
        return get_store<C>().get_dense();
    }

    template <typename C, typename = std::enable_if_t<is_valid_component_v<C>>>
    std::optional<std::reference_wrapper<C>> add_component(const EntityId id,
                                                           C &&component) {
        return get_store<C>().add(id, std::forward<C>(component));
    }

    template <typename C, typename = std::enable_if_t<is_valid_component_v<C>>>
    void update_component(const EntityId id, std::function<void(C &)> update_fn) {
        get_store<C>().update(id, update_fn);
    }

    template <typename C, typename = std::enable_if_t<is_valid_component_v<C>>>
    void apply_fn(std::function<void(EntityId, C &)> apply_fn) {
        auto &store = get_store<C>();
        for (auto it = store.begin(); it != store.end(); it++) {
            auto &component = *it;
            apply_fn(it.id(), component);
        }
    }

    template <typename C, typename = std::enable_if_t<is_valid_component_v<C>>>
    const size_t size() {
        return get_store<C>().size();
    }

    template <typename C, typename = std::enable_if_t<is_valid_component_v<C>>>
    ComponentStoreIterator<C> begin() {
        return get_store<C>().begin();
    }

    template <typename C, typename = std::enable_if_t<is_valid_component_v<C>>>
    ComponentStoreIterator<C> end() {
        return get_store<C>().end();
    }

    template <typename C, typename = std::enable_if_t<is_valid_component_v<C>>>
    ComponentStore<C> view() {
        return get_store<C>();
    }
};
