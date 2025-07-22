#pragma once
#include "render_engine/ui/ElementProperties.h"
#include "render_engine/ui/animations/Animation.h"

namespace ui {

template <typename T>
concept SupportedAnimationType = std::same_as<T, float> || std::same_as<T, glm::vec2> ||
                                 std::same_as<T, glm::vec3> || std::same_as<T, glm::vec4>;

class AnimationStore {
  private:
    template <typename T>
    using BuilderStore = std::unordered_map<std::string, AnimationBuildFn<T>>;
    BuilderStore<float> m_float_animation_builders;
    BuilderStore<glm::vec2> m_vec2_animation_builders;
    BuilderStore<glm::vec3> m_vec3_animation_builders;
    BuilderStore<glm::vec4> m_vec4_animation_builders;

    template <typename T> using Store = std::unordered_map<std::string, Animation<T>>;
    Store<float> m_float_animations;
    Store<glm::vec2> m_vec2_animations;
    Store<glm::vec3> m_vec3_animations;
    Store<glm::vec4> m_vec4_animations;

    template <typename T> constexpr auto &get_animation_store() {
        if constexpr (std::is_same_v<T, float>) {
            return m_float_animations;
        } else if constexpr (std::is_same_v<T, glm::vec2>) {
            return m_vec2_animations;
        } else if constexpr (std::is_same_v<T, glm::vec3>) {
            return m_vec3_animations;
        } else if constexpr (std::is_same_v<T, glm::vec4>) {
            return m_vec4_animations;
        } else {
            static_assert(false, "AnimationStore: Unsupported animation type");
            return m_float_animations;
        }
    }

    template <typename T> constexpr auto &get_animation_builder_store() {
        if constexpr (std::is_same_v<T, float>) {
            return m_float_animation_builders;
        } else if constexpr (std::is_same_v<T, glm::vec2>) {
            return m_vec2_animation_builders;
        } else if constexpr (std::is_same_v<T, glm::vec3>) {
            return m_vec3_animation_builders;
        } else if constexpr (std::is_same_v<T, glm::vec4>) {
            return m_vec4_animation_builders;
        } else {
            static_assert(false, "AnimationStore: Unsupported animation type");
            return m_float_animation_builders;
        }
    }

    template <typename T> constexpr void step_animations_() {
        auto &store = get_animation_store<T>();
        for (auto &[key, animation] : store) {
            animation.step();
        }
    }

    template <typename T> constexpr void rebuild_(ui::ElementProperties &props) {
        auto &builder_store = get_animation_builder_store<T>();
        auto &store = get_animation_store<T>();
        store.clear();
        for (auto &[key, builder] : builder_store) {
            store.insert({key, builder(props)});
        }
    }

  public:
    AnimationStore() = default;
    ~AnimationStore() = default;

    AnimationStore(const AnimationStore &other)
        : m_float_animation_builders(other.m_float_animation_builders),
          m_vec2_animation_builders(other.m_vec2_animation_builders),
          m_vec3_animation_builders(other.m_vec3_animation_builders),
          m_vec4_animation_builders(other.m_vec4_animation_builders),
          m_float_animations(), // Initialize empty
          m_vec2_animations(),  // Initialize empty
          m_vec3_animations(),  // Initialize empty
          m_vec4_animations()   // Initialize empty
    {}

    // Move constructor - move builders, leave animations empty
    AnimationStore(AnimationStore &&other) noexcept
        : m_float_animation_builders(std::move(other.m_float_animation_builders)),
          m_vec2_animation_builders(std::move(other.m_vec2_animation_builders)),
          m_vec3_animation_builders(std::move(other.m_vec3_animation_builders)),
          m_vec4_animation_builders(std::move(other.m_vec4_animation_builders)),
          m_float_animations(), // Initialize empty
          m_vec2_animations(),  // Initialize empty
          m_vec3_animations(),  // Initialize empty
          m_vec4_animations()   // Initialize empty
    {}

    AnimationStore &operator=(const AnimationStore &other) {
        if (this != &other) {
            m_float_animation_builders = other.m_float_animation_builders;
            m_vec2_animation_builders = other.m_vec2_animation_builders;
            m_vec3_animation_builders = other.m_vec3_animation_builders;
            m_vec4_animation_builders = other.m_vec4_animation_builders;

            m_float_animations.clear();
            m_vec2_animations.clear();
            m_vec3_animations.clear();
            m_vec4_animations.clear();
        }
        return *this;
    }

    AnimationStore &operator=(AnimationStore &&other) noexcept {
        if (this != &other) {
            // Move builders
            m_float_animation_builders = std::move(other.m_float_animation_builders);
            m_vec2_animation_builders = std::move(other.m_vec2_animation_builders);
            m_vec3_animation_builders = std::move(other.m_vec3_animation_builders);
            m_vec4_animation_builders = std::move(other.m_vec4_animation_builders);

            // Clear animations (don't move them as they are invalid)
            m_float_animations.clear();
            m_vec2_animations.clear();
            m_vec3_animations.clear();
            m_vec4_animations.clear();
        }
        return *this;
    }

    template <SupportedAnimationType T>
    void add(std::string &&key, AnimationBuildFn<T> &&builder, Animation<T> &&animation) {
        get_animation_builder_store<T>().insert({std::move(key), std::move(builder)});
        get_animation_store<T>().insert({std::move(key), std::move(animation)});
    }

    template <SupportedAnimationType T> Animation<T> &get(std::string &key) {
        return get_animation_store<T>()[key];
    }

    template <SupportedAnimationType T> Animation<T> &get(std::string &&key) {
        return get_animation_store<T>()[std::move(key)];
    }

    void rebuild(ui::ElementProperties &props) {
        rebuild_<float>(props);
        rebuild_<glm::vec2>(props);
        rebuild_<glm::vec3>(props);
        rebuild_<glm::vec4>(props);
    }

    void step_animations() {
        step_animations_<float>();
        step_animations_<glm::vec2>();
        step_animations_<glm::vec3>();
        step_animations_<glm::vec4>();
    }
};

} // namespace ui
