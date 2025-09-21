#pragma once

#include "game_engine_sdk/render_engine/ui/ElementProperties.h"
#include "game_engine_sdk/render_engine/ui/animations/AnimationCurve.h"
#include <cstdint>

namespace {
template <typename T> struct AnimateValue {
    T *value_to_animate;
    T target_value;
    AnimateValue(T *property, T target)
        : value_to_animate(property), target_value(target) {}
};
} // namespace

namespace ui {

/**
 * @brief Enum class defining behavior when an animation completes
 */
enum class OnAnimationCompleted { STOP, REVERSE, LOOP };

/**
 * @brief Template class for animating values of any numeric type
 *
 * @tparam T The type of value to animate (must support arithmetic operations)
 *
 * @details The Animation class provides a flexible animation system that can interpolate
 * between two values of any numeric type using various animation curves. It supports
 * different completion policies and can be controlled manually or automatically.
 *
 * The class uses the builder pattern for construction via the AnimationBuilder friend
 class.
 *
 * @note This class assumes T supports:
 * - Default construction
 * - Copy construction and assignment
 * - Arithmetic operations (addition, subtraction, multiplication)
 *
 * @example
 * ```cpp
 * auto anim = ui::AnimationBuilder()
 *     .set_duration(600)
 *     .set_on_completed(ui::OnAnimationCompleted::STOP)
 *     .set_animation_curve(ui::AnimationCurve::linear)
 *     .build(&props.border.color, colors::BLUE);
 * ```
 */
template <typename T> class Animation {
  private:
    friend class AnimationBuilder;

    bool m_is_playing;
    AnimationCurve::AnimationCurve m_animation_curve;
    OnAnimationCompleted m_on_completed;
    int32_t m_num_steps;
    int32_t m_step_count;
    int32_t m_delta_step;
    float m_direction;
    float m_current_x;
    float m_delta_x;
    T m_original_value;
    T m_target_value;
    T *m_current_value;
    T m_delta;

    Animation(AnimateValue<T> &&value, AnimationCurve::AnimationCurve curve,
              uint32_t num_frames, OnAnimationCompleted on_completed)
        : m_is_playing(false), m_animation_curve(curve), m_on_completed(on_completed),
          m_num_steps(num_frames), m_step_count(0), m_delta_step(1), m_current_x(0.0f),
          m_delta_x(1.0f / num_frames), m_original_value(*value.value_to_animate),
          m_current_value(value.value_to_animate), m_target_value(value.target_value),
          m_delta(m_target_value - m_original_value) {}

    void act_based_on_completed_policy() {
        switch (m_on_completed) {
        case ui::OnAnimationCompleted::LOOP:
            reset();
            break;
        case ui::OnAnimationCompleted::REVERSE:
            reverse();
            break;
        case ui::OnAnimationCompleted::STOP:
            stop();
            break;
        };
    }

  public:
    /**
     * @brief Default constructor
     * @details Creates an uninitialized animation. Must be assigned from a valid
     * animation.
     */
    Animation() = default;

    /**
     * @brief Destructor
     */
    ~Animation() = default;

    /**
     * @brief Move constructor
     * @param other Animation to move from
     */
    Animation(Animation &&other) noexcept = default;

    /**
     * @brief Copy constructor
     * @param other Animation to copy from
     * @warning Copying shares the same target value pointer
     */
    Animation(const Animation &other) = default;

    /**
     * @brief Move assignment operator
     * @param other Animation to move from
     * @return Reference to this animation
     */
    Animation &operator=(Animation &&other) = default;

    /**
     * @brief Copy assignment operator
     * @param other Animation to copy from
     * @return Reference to this animation
     * @warning Copying shares the same target value pointer
     */
    Animation &operator=(const Animation &other) = default;

    /**
     * @brief Manually advance the animation by specified steps
     *
     * @param n_steps Number of steps to advance
     *
     * @details Advances the animation and updates the interpolated value using
     * the animation curve. Checks for completion and applies completion policy.
     * Only processes if the animation is currently playing.
     */
    void step(size_t n_steps) {

        if (!m_is_playing) {
            return;
        }

        m_current_x += m_delta_x;
        float interpolated = m_animation_curve(m_current_x);
        *m_current_value = m_original_value + m_delta * interpolated;
        m_step_count += m_delta_step;

        if (m_step_count <= 0 || m_num_steps <= m_step_count) {
            act_based_on_completed_policy();
        }
    }

    /**
     * @brief Manually advance the animation by one step
     *
     * @details Convenience method that advances the animation by a single step.
     * Updates the interpolated value and checks for completion.
     */
    void step() { step(1); }

    /**
     * @brief Reset animation to initial state
     *
     * @details Sets the animated value back to its original value,
     * resets the interpolation position to 0.0, and resets the step counter.
     * Does not affect the playing state.
     */
    void reset() {
        *m_current_value = m_original_value;
        m_current_x = 0.0f;
        m_step_count = 0;
    }

    /**
     * @brief Start automatic animation playback
     *
     * @return Reference to this animation for method chaining
     *
     * @details Sets the animation to playing state. The animation will continue
     * to interpolate when step() is called until stop() is called or completion
     * policy is triggered.
     */
    Animation &play() {
        m_is_playing = true;
        return *this;
    }

    /**
     * @brief Toggle animation playback state
     *
     * @return Reference to this animation for method chaining
     *
     * @details Switches between playing and stopped states.
     * If currently playing, stops the animation.
     * If currently stopped, starts the animation.
     */
    Animation &toggle() {
        m_is_playing = !m_is_playing;
        return *this;
    }

    /**
     * @brief Reverse the animation direction
     *
     * @details Changes the animation to play in the opposite direction.
     * The animation will interpolate from current position toward the original value.
     */
    void reverse() {
        m_delta_x *= -1.0f;
        m_delta_step *= -1;
    }

    /**
     * @brief Stop automatic animation playback
     *
     * @return Reference to this animation for method chaining
     *
     * @details Sets the animation to stopped state. The animation will no longer
     * update when step() is called until play() is called again.
     * Does not reset the animation position.
     */
    Animation &stop() {
        m_is_playing = false;
        return *this;
    }
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
