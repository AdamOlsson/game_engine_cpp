#pragma once

#include "render_engine/ui/Animation.h"
#include "render_engine/ui/ElementProperties.h"
#include <functional>

namespace ui {

enum class ButtonClickState { Pressed, Unpressed };
enum class ButtonCursorState { Outside, Enter, Hover, Leave };

class Button {
  public:
    ElementProperties properties;
    ButtonClickState click_state;
    ButtonCursorState cursor_state;

    std::function<void(ui::Button &)> on_click;
    std::function<void(ui::Button &)> on_unclick;
    std::function<void(ui::Button &)> on_hover;
    std::function<void(ui::Button &)> on_enter;
    std::function<void(ui::Button &)> on_leave;

    Button() = default;

    Button(ElementProperties &&properties)
        : properties(properties), click_state(ButtonClickState::Unpressed),
          cursor_state(ButtonCursorState::Outside), on_click([](ui::Button &b) {}),
          on_unclick([](ui::Button &b) {}), on_hover([](ui::Button &b) {}),
          on_enter([](ui::Button &b) {}), on_leave([](ui::Button &b) {}) {}

    Button(ElementProperties &properties)
        : properties(properties), click_state(ButtonClickState::Unpressed),
          cursor_state(ButtonCursorState::Outside), on_click([](ui::Button &b) {}),
          on_unclick([](ui::Button &b) {}), on_hover([](ui::Button &b) {}),
          on_enter([](ui::Button &b) {}), on_leave([](ui::Button &b) {}) {}

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

    template <typename T>
    Button &add_animation(
        std::function<ui::Animation<T>(ui::ElementProperties &)> animation_builder) {
        // TODO: Some kind of animation storage like the ECS
    };
};
} // namespace ui
