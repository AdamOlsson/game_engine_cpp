#include "render_engine/colors.h"
#include "render_engine/ui/Button.h"
#include "render_engine/ui/UI.h"
#include "render_engine/window/ViewportPoint.h"
#include <gtest/gtest.h>

TEST(UITest, TestOnEnterAndOnLeaveWithSingleButton) {
    const auto cursor_pos_on_button = window::ViewportPoint(0.0f, 0.0f);
    const auto cursor_pos_off_button = window::ViewportPoint(200.1f, 0.0f);
    auto m_ui = ui::UI(ui::Menu().add_button(
        ui::Button("",
                   ui::ElementProperties{
                       .container.center = glm::vec2(0.0f, 0.0f),
                       .container.dimension = glm::vec2(400.0f, 100.0f),
                       .container.border.color = colors::GREEN,
                       .container.border.thickness = 5.0f,
                       .container.border.radius = 15.0f,
                   })
            .set_on_enter([](ui::Button &self) {
                self.properties.container.border.color = colors::BLUE;
            })
            .set_on_leave([](ui::Button &self) {
                self.properties.container.border.color = colors::RED;
            })));

    const ui::State &initial_state = m_ui.get_state();

    // Validate initial state
    ASSERT_EQ(initial_state.buttons.size(), 1);
    ASSERT_EQ(initial_state.buttons[0]->properties.container.border.color, colors::GREEN);

    m_ui.update_state_from_mouse_event(window::MouseEvent::CURSOR_MOVED,
                                       cursor_pos_on_button);
    const ui::State state_1 = m_ui.get_state();

    // Validate state 1
    ASSERT_EQ(state_1.buttons.size(), 1);
    ASSERT_EQ(state_1.buttons[0]->properties.container.border.color, colors::BLUE);

    m_ui.update_state_from_mouse_event(window::MouseEvent::CURSOR_MOVED,
                                       cursor_pos_off_button);
    const ui::State state_2 = m_ui.get_state();

    // Validate state 2
    ASSERT_EQ(state_2.buttons.size(), 1);
    ASSERT_EQ(state_2.buttons[0]->properties.container.border.color, colors::RED);
}

TEST(UITest, TestOnEnterAndOnLeaveWithThreeeButtons) {
    const auto cursor_pos_on_button = window::ViewportPoint(0.0f, 0.0f);
    const auto cursor_pos_off_button = window::ViewportPoint(200.1f, 0.0f);
    auto m_ui = ui::UI(
        ui::Menu()
            .add_button(ui::Button("",
                                   ui::ElementProperties{
                                       .container.center = glm::vec2(0.0f, 200.0f),
                                       .container.dimension = glm::vec2(400.0f, 100.0f),
                                       .container.border.color = colors::DARK_GREEN,
                                       .container.border.thickness = 5.0f,
                                       .container.border.radius = 15.0f,
                                   })
                            .set_on_enter([](ui::Button &self) {
                                self.properties.container.border.color =
                                    colors::DARK_BLUE;
                            })
                            .set_on_leave([](ui::Button &self) {
                                self.properties.container.border.color = colors::DARK_RED;
                            }))
            .add_button(ui::Button("",
                                   ui::ElementProperties{
                                       .container.center = glm::vec2(0.0f, 0.0f),
                                       .container.dimension = glm::vec2(400.0f, 100.0f),
                                       .container.border.color = colors::GREEN,
                                       .container.border.thickness = 5.0f,
                                       .container.border.radius = 15.0f,
                                   })
                            .set_on_enter([](ui::Button &self) {
                                self.properties.container.border.color = colors::BLUE;
                            })
                            .set_on_leave([](ui::Button &self) {
                                self.properties.container.border.color = colors::RED;
                            }))
            .add_button(ui::Button("",
                                   ui::ElementProperties{
                                       .container.center = glm::vec2(0.0f, -200.0f),
                                       .container.dimension = glm::vec2(400.0f, 100.0f),
                                       .container.border.color = colors::LIGHT_GREEN,
                                       .container.border.thickness = 5.0f,
                                       .container.border.radius = 15.0f,
                                   })
                            .set_on_enter([](ui::Button &self) {
                                self.properties.container.border.color =
                                    colors::LIGHT_BLUE;
                            })
                            .set_on_leave([](ui::Button &self) {
                                self.properties.container.border.color =
                                    colors::LIGHT_RED;
                            }))

    );

    const ui::State initial_state = m_ui.get_state();

    // Validate initial state
    ASSERT_EQ(initial_state.buttons.size(), 3);
    ASSERT_EQ(initial_state.buttons[0]->properties.container.border.color,
              colors::DARK_GREEN);
    ASSERT_EQ(initial_state.buttons[1]->properties.container.border.color, colors::GREEN);
    ASSERT_EQ(initial_state.buttons[2]->properties.container.border.color,
              colors::LIGHT_GREEN);

    m_ui.update_state_from_mouse_event(window::MouseEvent::CURSOR_MOVED,
                                       cursor_pos_on_button);
    const ui::State state_1 = m_ui.get_state();

    // Validate state 1
    ASSERT_EQ(state_1.buttons.size(), 3);
    ASSERT_EQ(initial_state.buttons[0]->properties.container.border.color,
              colors::DARK_GREEN);
    ASSERT_EQ(initial_state.buttons[1]->properties.container.border.color, colors::BLUE);
    ASSERT_EQ(initial_state.buttons[2]->properties.container.border.color,
              colors::LIGHT_GREEN);

    m_ui.update_state_from_mouse_event(window::MouseEvent::CURSOR_MOVED,
                                       cursor_pos_off_button);
    const ui::State state_2 = m_ui.get_state();

    // Validate state 2
    ASSERT_EQ(state_2.buttons.size(), 3);
    ASSERT_EQ(initial_state.buttons[0]->properties.container.border.color,
              colors::DARK_GREEN);
    ASSERT_EQ(initial_state.buttons[1]->properties.container.border.color, colors::RED);
    ASSERT_EQ(initial_state.buttons[2]->properties.container.border.color,
              colors::LIGHT_GREEN);
}
