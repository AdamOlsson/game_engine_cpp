#pragma once

#include "render_engine/ui/Animation.h"
#include "render_engine/ui/ElementProperties.h"
#include <functional>

namespace ui {

enum class ButtonClickState { Pressed, Unpressed };
enum class ButtonCursorState { Outside, Enter, Hover, Leave };

// Note that every time a Button instance is moved or copied, all animations on that
// Button are rebuilt. This has to do with that the new button instances animations
// contains pointers to the old button instances element properties. To properly handles
// this we rebuild the animation so that the new button instances animations point to the
// new buttons instances element properties. This behavior can lead to perfomance loss if
// done in the main loop of the game.
class Button {
  private:
    std::vector<std::function<ui::Animation<glm::vec2>(ui::Button &)>>
        m_animation_builders;
    void build_animations() {
        for (auto &builder : m_animation_builders) {
            animations.push_back(builder(*this));
        }
    }

  public:
    ElementProperties properties;
    ButtonClickState click_state;
    ButtonCursorState cursor_state;

    std::vector<Animation<glm::vec2>> animations;

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

    Button(const Button &other)
        : properties(other.properties), click_state(other.click_state),
          cursor_state(other.cursor_state), on_click(other.on_click),
          on_unclick(other.on_unclick), on_hover(other.on_hover),
          on_enter(other.on_enter), on_leave(other.on_leave),
          m_animation_builders(other.m_animation_builders) {
        build_animations();
    }

    Button &operator=(const Button &other) {
        if (this != &other) {
            properties = other.properties;
            click_state = other.click_state;
            cursor_state = other.cursor_state;
            animations = {}; // Animations are rebuilt
            on_click = other.on_click;
            on_unclick = other.on_unclick;
            on_hover = other.on_hover;
            on_enter = other.on_enter;
            on_leave = other.on_leave;

            m_animation_builders = other.m_animation_builders;

            build_animations();
        }
        return *this;
    }

    Button(Button &&other) noexcept
        : properties(other.properties), click_state(std::move(other.click_state)),
          cursor_state(std::move(other.cursor_state)), animations({}),
          on_click(std::move(other.on_click)), on_unclick(std::move(other.on_unclick)),
          on_hover(std::move(other.on_hover)), on_enter(std::move(other.on_enter)),
          on_leave(std::move(other.on_leave)),
          m_animation_builders(std::move(other.m_animation_builders)) {
        other.on_click = [](ui::Button &b) {};
        other.on_unclick = [](ui::Button &b) {};
        other.on_hover = [](ui::Button &b) {};
        other.on_enter = [](ui::Button &b) {};
        other.on_leave = [](ui::Button &b) {};
        build_animations();
    }

    Button &operator=(Button &&other) noexcept {
        if (this != &other) {
            properties = std::move(other.properties);
            click_state = std::move(other.click_state);
            cursor_state = std::move(other.cursor_state);
            animations = {}; // Animations are rebuilt
            on_click = std::move(other.on_click);
            on_unclick = std::move(other.on_unclick);
            on_hover = std::move(other.on_hover);
            on_enter = std::move(other.on_enter);
            on_leave = std::move(other.on_leave);

            m_animation_builders = std::move(other.m_animation_builders);

            other.on_click = [](ui::Button &b) {};
            other.on_unclick = [](ui::Button &b) {};
            other.on_hover = [](ui::Button &b) {};
            other.on_enter = [](ui::Button &b) {};
            other.on_leave = [](ui::Button &b) {};

            build_animations();
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

    Button &add_animation(
        std::function<ui::Animation<glm::vec2>(ui::Button &)> animation_builder) {
        animations.push_back(animation_builder(*this));
        m_animation_builders.push_back(animation_builder);
        return *this;
    };

    void increment_animations() {
        for (auto &anim : animations) {
            anim.increment();
        }
    }
};
} // namespace ui
