#pragma once

#include "render_engine/ui/ElementProperties.h"
#include "render_engine/ui/animations/AnimationCurve.h"
#include <cstdint>

namespace ui {
enum class OnAnimationCompleted { STOP, REVERSE, LOOP };

template <typename T> struct AnimateValue {
    T *value_to_animate;
    T target_value;
    AnimateValue(T *property, T target)
        : value_to_animate(property), target_value(target) {}
};

template <typename T> class Animation {
  private:
    friend class AnimationBuilder;

    AnimationCurve::AnimationCurve m_animation_curve;
    OnAnimationCompleted m_on_completed;
    float m_current_x;
    float m_delta_x;
    T m_original_value;
    T m_target_value;
    T *m_current_value;
    T m_delta;

    Animation(AnimateValue<T> &&value, AnimationCurve::AnimationCurve curve,
              uint32_t num_frames, OnAnimationCompleted on_completed)
        : m_animation_curve(curve), m_on_completed(on_completed), m_current_x(0.0f),
          m_delta_x(1.0f / num_frames), m_original_value(*value.value_to_animate),
          m_current_value(value.value_to_animate), m_target_value(value.target_value),
          m_delta(m_target_value - m_original_value) {}

  public:
    Animation() = default;
    ~Animation() = default;
    Animation(Animation &&other) noexcept = default;
    Animation(const Animation &other) = default;
    Animation &operator=(Animation &&other) = default;
    Animation &operator=(const Animation &other) = default;

    void reset() { *m_current_value = m_original_value; }

    // Manually increment the animation
    void step(size_t n_steps) {
        // TODO: Take m_on_completed into consideration and act accordingly
        m_current_x += m_delta_x;
        float intepolated = m_animation_curve(m_current_x);
        *m_current_value = m_original_value + m_delta * intepolated;
    }

    void step() { step(1); }

    // Manually decrement the animation
    void step_backward(size_t n) {}
    void step_backward() { step_backward(1); }

    // Automatically play the animation until stop is called
    Animation &play() { return *this; }

    // Automatically play the animation in reverse until stop is called
    void play_reverse() {}

    // Stop playing the animation automatically
    void stop() {}
};

template <typename T>
using AnimationBuildFn = std::function<ui::Animation<T>(ui::ElementProperties &)>;

class AnimationBuilder {
  private:
    OnAnimationCompleted m_on_animation_completed = OnAnimationCompleted::STOP;
    AnimationCurve::AnimationCurve m_animation_curve = AnimationCurve::linear;
    uint32_t m_duration_in_frames = 1;

  public:
    AnimationBuilder() = default;
    ~AnimationBuilder() = default;

    AnimationBuilder &set_on_completed(OnAnimationCompleted on_completed) {
        m_on_animation_completed = on_completed;
        return *this;
    }

    AnimationBuilder &set_duration(uint32_t num_frames) {
        m_duration_in_frames = num_frames;
        return *this;
    }

    AnimationBuilder &
    set_animation_curve(AnimationCurve::AnimationCurve animation_curve) {
        m_animation_curve = animation_curve;
        return *this;
    }

    template <typename T> Animation<T> build(T *property, const T &&target_value) {
        return Animation<T>(AnimateValue<T>(property, target_value), m_animation_curve,
                            m_duration_in_frames, m_on_animation_completed);
    }

    template <typename T> Animation<T> build(T *property, const T &target_value) {
        return Animation<T>(AnimateValue<T>(property, target_value), m_animation_curve,
                            m_duration_in_frames, m_on_animation_completed);
    }
};

} // namespace ui
