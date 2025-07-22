#pragma once

#include "render_engine/ui/ElementProperties.h"
#include "render_engine/ui/animations/Animation.h"
#include "render_engine/ui/animations/AnimationStore.h"
#include <functional>

namespace ui {

enum class ButtonClickState { Pressed, Unpressed };
enum class ButtonCursorState { Outside, Enter, Hover, Leave };

template <typename> struct extract_animation_type;
template <typename T> struct extract_animation_type<ui::Animation<T>> {
    using type = T;
};

// Note that every time a Button instance is moved or copied, all animations on that
// Button are rebuilt. This has to do with that the new button instances animations
// contains pointers to the old button instances element properties. To properly
// handles this we rebuild the animation so that the new button instances animations
// point to the new buttons instances element properties. This behavior can lead to
// perfomance loss if done in the main loop of the game. To resolve this issue, I need to
// refactor the Animations. The problem is that the animations use a pointer to an
// ElementProperties struct member, which becomes invalid after moving. I should use a
// lambda function based system that receives the entire button (or the buttons
// ElementProperties). This also requires some thinking about how to do this nice from the
// users perspective.
class Button {
  public:
    std::string text;
    ElementProperties properties;
    ButtonClickState click_state;
    ButtonCursorState cursor_state;

    AnimationStore animations;

    std::function<void(ui::Button &)> on_click;
    std::function<void(ui::Button &)> on_unclick;
    std::function<void(ui::Button &)> on_hover;
    std::function<void(ui::Button &)> on_enter;
    std::function<void(ui::Button &)> on_leave;

    Button() = default;

    Button(std::string &&text, ElementProperties &&properties)
        : text(std::move(text)), properties(properties),
          click_state(ButtonClickState::Unpressed),
          cursor_state(ButtonCursorState::Outside), on_click([](ui::Button &b) {}),
          on_unclick([](ui::Button &b) {}), on_hover([](ui::Button &b) {}),
          on_enter([](ui::Button &b) {}), on_leave([](ui::Button &b) {}) {}

    Button(std::string &text, ElementProperties &properties)
        : text(text), properties(properties), click_state(ButtonClickState::Unpressed),
          cursor_state(ButtonCursorState::Outside), on_click([](ui::Button &b) {}),
          on_unclick([](ui::Button &b) {}), on_hover([](ui::Button &b) {}),
          on_enter([](ui::Button &b) {}), on_leave([](ui::Button &b) {}) {}

    Button(const Button &other)
        : text(other.text), properties(other.properties), click_state(other.click_state),
          cursor_state(other.cursor_state), on_click(other.on_click),
          on_unclick(other.on_unclick), on_hover(other.on_hover),
          on_enter(other.on_enter), on_leave(other.on_leave),
          animations(other.animations) {}

    Button &operator=(const Button &other) {
        if (this != &other) {
            text = other.text;
            properties = other.properties;
            click_state = other.click_state;
            cursor_state = other.cursor_state;
            animations = other.animations;
            on_click = other.on_click;
            on_unclick = other.on_unclick;
            on_hover = other.on_hover;
            on_enter = other.on_enter;
            on_leave = other.on_leave;
            animations.rebuild(properties);
        }
        return *this;
    }

    Button(Button &&other) noexcept
        : text(std::move(other.text)), properties(std::move(other.properties)),
          click_state(std::move(other.click_state)),
          cursor_state(std::move(other.cursor_state)),
          animations(std::move(other.animations)), on_click(std::move(other.on_click)),
          on_unclick(std::move(other.on_unclick)), on_hover(std::move(other.on_hover)),
          on_enter(std::move(other.on_enter)), on_leave(std::move(other.on_leave)) {
        other.on_click = [](ui::Button &b) {};
        other.on_unclick = [](ui::Button &b) {};
        other.on_hover = [](ui::Button &b) {};
        other.on_enter = [](ui::Button &b) {};
        other.on_leave = [](ui::Button &b) {};
        animations.rebuild(properties);
    }

    Button &operator=(Button &&other) noexcept {
        if (this != &other) {
            text = std::move(other.text);
            properties = std::move(other.properties);
            click_state = std::move(other.click_state);
            cursor_state = std::move(other.cursor_state);
            animations = std::move(other.animations);
            on_click = std::move(other.on_click);
            on_unclick = std::move(other.on_unclick);
            on_hover = std::move(other.on_hover);
            on_enter = std::move(other.on_enter);
            on_leave = std::move(other.on_leave);
            animations.rebuild(properties);

            other.on_click = [](ui::Button &b) {};
            other.on_unclick = [](ui::Button &b) {};
            other.on_hover = [](ui::Button &b) {};
            other.on_enter = [](ui::Button &b) {};
            other.on_leave = [](ui::Button &b) {};
        }
        return *this;
    }

    Button &set_on_click(std::function<void(ui::Button &)> on_click) {
        this->on_click = on_click;
        return *this;
    }

    Button &set_on_unclick(std::function<void(ui::Button &)> on_unclick) {
        this->on_unclick = on_unclick;
        return *this;
    }

    Button &set_on_hover(std::function<void(ui::Button &)> on_hover) {
        this->on_hover = on_hover;
        return *this;
    }

    Button &set_on_enter(std::function<void(ui::Button &)> on_enter) {
        this->on_enter = on_enter;
        return *this;
    }

    Button &set_on_leave(std::function<void(ui::Button &)> on_leave) {
        this->on_leave = on_leave;
        return *this;
    }

    template <typename AnimationBuilderFn>
    Button &add_animation(std::string &&id, AnimationBuilderFn animation_builder) {
        using AnimationType =
            std::invoke_result_t<AnimationBuilderFn, ui::ElementProperties &>;
        using T = typename extract_animation_type<AnimationType>::type;

        animations.add<T>(std::move(id), std::move(animation_builder),
                          animation_builder(properties));
        return *this;
    }
};
} // namespace ui
