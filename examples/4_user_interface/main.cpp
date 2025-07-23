#include "Game.h"
#include "GameEngine.h"
#include "render_engine/PerformanceWindow.h"
#include "render_engine/colors.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/ui/Button.h"
#include "render_engine/ui/ElementProperties.h"
#include "render_engine/ui/UI.h"
#include "render_engine/window/WindowConfig.h"
#include <memory>

// FUTURE IMPROVEMENTS (without any particular order):
// - 1. Animations should not use a pointer to the member of ElementProperties it wants
// to animate, as keeping pointer to struct members is bad practise. Instead, I should use
// a lambda function based system that act on either the ui element or on the
// ElementProperties struct.
// - 2. I want to add TextBox class to the menu hierarchy. Right now, if I do not want to
// render some text I need to set it to an empty string.
// - 3. It is not possible to call RenderEngine::render_text() and
// RenderEngine::render_ui() in the same iteration due to that they overwrite each others
// gpu buffers. I want to be able to render text without having the UI class manage them.

void on_enter_callback(ui::Button &self) {
    self.properties.container.background_color = colors::WHITE;
    self.properties.font.color = colors::BLACK;
}

void on_leave_callback(ui::Button &self) {
    self.properties.container.background_color = colors::BLACK;
    self.properties.font.color = colors::WHITE;
}

class UserInterfaceExample : public Game {
  private:
    ui::UI m_ui;

    const std::string VERSION_ID = "VERSION";
    const std::string NUMBER_ID = "NUMBER";
    const std::string INCREMENT_ID = "INCREMENT";
    int m_number;
    int m_increment;
    bool m_in_settings;

    const float base_width = 400.0f;
    const glm::vec2 top_button_pos = glm::vec2(0.0f, 50.0f);
    const glm::vec2 button_dimension = glm::vec2(base_width, 100.0f);
    const glm::vec2 square_button_dimension = glm::vec2(190.0f, 100.0f);
    const glm::vec2 square_button_offset =
        glm::vec2((base_width - square_button_dimension.x) / 2.0, 0.0f);
    const glm::vec2 next_button_offset = glm::vec2(0.0f, 120.0f);
    const glm::vec4 button_background_color = colors::BLACK;
    const glm::vec4 button_font_color = colors::WHITE;
    const uint32_t button_font_size = 96;
    const glm::vec4 button_border_color = colors::WHITE;
    const float button_border_thickness = 5.0f;
    const float button_border_radius = 15.0f;

    const glm::vec2 square_settings_button_dimension = glm::vec2(125.0f, 100.0f);
    const glm::vec2 square_settings_button_offset =
        glm::vec2((base_width - square_settings_button_dimension.x) / 2.0, 0.0f);

  public:
    UserInterfaceExample() : m_number(0), m_increment(1), m_in_settings(false) {
        m_ui = ui::UI(
            ui::Menu()
                .add_button(
                    ui::Button(
                        "+",
                        ui::ElementProperties{
                            .container.center = top_button_pos + square_button_offset,
                            .container.dimension = square_button_dimension,
                            .container.background_color = button_background_color,
                            .container.border.color = button_border_color,
                            .container.border.thickness = button_border_thickness,
                            .container.border.radius = button_border_radius,
                            .font.color = button_font_color,
                            .font.size = button_font_size})
                        .set_on_enter(on_enter_callback)
                        .set_on_leave(on_leave_callback)
                        .set_on_click([this](ui::Button &self) {
                            this->m_number += this->m_increment;
                        }))

                .add_button(
                    ui::Button("-",
                               ui::ElementProperties{
                                   .container.center = top_button_pos -
                                                       square_button_offset,
                                   .container.dimension = square_button_dimension,
                                   .container.background_color = button_background_color,
                                   .container.border.color = button_border_color,
                                   .container.border.thickness = button_border_thickness,
                                   .container.border.radius = button_border_radius,
                                   .font.color = button_font_color,
                                   .font.size = button_font_size})
                        .set_on_enter(on_enter_callback)
                        .set_on_leave(on_leave_callback)
                        .set_on_click([this](ui::Button &self) {
                            this->m_number -= this->m_increment;
                        }))

                .add_submenu(
                    ui::Menu(
                        ui::Button("SETTINGS",
                                   ui::ElementProperties{
                                       .container.center = top_button_pos -
                                                           next_button_offset * 1.0f,
                                       .container.dimension = button_dimension,
                                       .container.background_color =
                                           button_background_color,
                                       .container.border.color = button_border_color,
                                       .container.border.thickness =
                                           button_border_thickness,
                                       .container.border.radius = button_border_radius,
                                       .font.color = button_font_color,
                                       .font.size = button_font_size})
                            .set_on_enter(on_enter_callback)
                            .set_on_leave(on_leave_callback)
                            .set_on_click(
                                [this](ui::Button &self) { this->m_in_settings = true; }),

                        ui::Button(
                            "BACK",
                            ui::ElementProperties{
                                .container.center =
                                    top_button_pos - next_button_offset * 1.0f,
                                .container.dimension = button_dimension,
                                .container.background_color = button_background_color,
                                .container.border.color = button_border_color,
                                .container.border.thickness = button_border_thickness,
                                .container.border.radius = button_border_radius,
                                .font.color = button_font_color,
                                .font.size = button_font_size})
                            .set_on_enter(on_enter_callback)
                            .set_on_leave(on_leave_callback)
                            .set_on_click([this](ui::Button &self) {
                                this->m_in_settings = false;
                            }))

                        .add_button(
                            ui::Button(
                                "+",
                                ui::ElementProperties{
                                    .container.center =
                                        top_button_pos + square_settings_button_offset,
                                    .container.dimension =
                                        square_settings_button_dimension,
                                    .container.background_color = button_background_color,
                                    .container.border.color = button_border_color,
                                    .container.border.thickness = button_border_thickness,
                                    .container.border.radius = button_border_radius,
                                    .font.color = button_font_color,
                                    .font.size = button_font_size})
                                .set_on_enter(on_enter_callback)
                                .set_on_leave(on_leave_callback)
                                .set_on_click(
                                    [this](ui::Button &self) { this->m_increment++; }))

                        .add_button(
                            ui::Button(
                                "-",
                                ui::ElementProperties{
                                    .container.center =
                                        top_button_pos - square_settings_button_offset,
                                    .container.dimension =
                                        square_settings_button_dimension,
                                    .container.background_color = button_background_color,
                                    .container.border.color = button_border_color,
                                    .container.border.thickness = button_border_thickness,
                                    .container.border.radius = button_border_radius,
                                    .font.color = button_font_color,
                                    .font.size = button_font_size})
                                .set_on_enter(on_enter_callback)
                                .set_on_leave(on_leave_callback)
                                .set_on_click(
                                    [this](ui::Button &self) { this->m_increment--; })))

                .add_button(
                    ui::Button("EXIT",
                               ui::ElementProperties{
                                   .container.center =
                                       top_button_pos - next_button_offset * 2.0f,
                                   .container.dimension = button_dimension,
                                   .container.background_color = button_background_color,
                                   .container.border.color = button_border_color,
                                   .container.border.thickness = button_border_thickness,
                                   .container.border.radius = button_border_radius,
                                   .font.color = button_font_color,
                                   .font.size = button_font_size})
                        .set_on_enter(on_enter_callback)
                        .set_on_leave(on_leave_callback)
                        .set_on_click([](ui::Button &self) { exit(0); })));

        m_ui.add_text_box(
            NUMBER_ID,
            ui::TextBox(std::to_string(m_number),
                        ui::ElementProperties{.container.center = glm::vec2(0.0f, 250.0f),
                                              .font.size = 158}));
        m_ui.add_text_box(
            INCREMENT_ID,
            ui::TextBox(std::to_string(m_increment),
                        ui::ElementProperties{.container.center = top_button_pos,
                                              .font.size = 48}));

        m_ui.add_text_box(VERSION_ID,
                          ui::TextBox("VERSION 1.234",
                                      ui::ElementProperties{
                                          .container.center = glm::vec2(-330.0f, -388.0f),
                                          .font.size = 24}));
    };

    ~UserInterfaceExample() {};

    void update(float dt) override {};

    void render(RenderEngine &render_engine) override {
        bool success = render_engine.begin_render_pass();
        if (!success) {
            return;
        }

        if (m_in_settings) {
            m_ui.get_text_box(INCREMENT_ID).text = std::to_string(m_increment);
        } else {
            m_ui.get_text_box(INCREMENT_ID).text = "";
        }

        m_ui.get_text_box(NUMBER_ID).text = std::to_string(m_number);

        auto ui_state = m_ui.get_state();

        render_engine.render_ui(ui_state);

        PerformanceWindow::get_instance().render(render_engine);

        success = render_engine.end_render_pass();
        if (!success) {
            return;
        }
    };

    void setup(RenderEngine &render_engine) override {
        render_engine.register_mouse_event_callback(
            [this](window::MouseEvent e, window::ViewportPoint &p) {
                this->m_ui.update_state_from_mouse_event(e, p);
            });
    }
};

int main() {

    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(800, 800),
                                              .title = "4_user_interface"},
        .use_font = UseFont::Default,
    };

    auto game = std::make_unique<UserInterfaceExample>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
