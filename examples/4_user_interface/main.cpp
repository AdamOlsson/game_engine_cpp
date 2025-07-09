#include "Coordinates.h"
#include "Game.h"
#include "GameEngine.h"
#include "render_engine/WindowConfig.h"
#include "render_engine/colors.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/ui/Button.h"
#include "render_engine/ui/ElementProperties.h"
#include "render_engine/ui/UI.h"
#include <memory>

void on_click_callback(ui::Button &self) { std::cout << "Click!" << std::endl; }

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
    size_t m_number;

    const glm::vec2 top_button_pos = glm::vec2(0.0f, -50.0f);
    const glm::vec2 button_dimension = glm::vec2(400.0f, 100.0f);
    const glm::vec2 square_button_dimension = glm::vec2(190.0f, 100.0f);
    const glm::vec2 square_button_offset = glm::vec2(82.5f + 20.0f, 0.0f);
    const glm::vec2 next_button_offset = glm::vec2(0.0f, 120.0f);
    const glm::vec4 button_background_color = colors::BLACK;
    const glm::vec4 button_font_color = colors::WHITE;
    const uint32_t button_font_size = 96;
    const glm::vec4 button_border_color = colors::WHITE;
    const float button_border_thickness = 5.0f;
    const float button_border_radius = 15.0f;

  public:
    UserInterfaceExample() : m_number(0) {

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
                        .set_on_click([](ui::Button &self) {})
                    /*.add_animation("animation1",*/
                    /*               [](ui::ElementProperties &props) {*/
                    /*                   return ui::AnimationBuilder()*/
                    /*                       .set_duration(300)*/
                    /*                       .set_animation_curve(*/
                    /*                           ui::AnimationCurve::smoothstep)*/
                    /*                       .set_on_completed(*/
                    /*                           ui::OnAnimationCompleted::LOOP)*/
                    /*                       .build(&props.container.center,*/
                    /*                              glm::vec2(100.0f, -250.0))*/
                    /*                       .play();*/
                    /*               })*/
                    )

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
                        .set_on_click([](ui::Button &self) {}))

                .add_submenu(
                    ui::Menu(
                        ui::Button("SETTINGS",
                                   ui::ElementProperties{
                                       .container.center = top_button_pos +
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
                            .set_on_click(on_click_callback),

                        ui::Button(
                            "BACK",
                            ui::ElementProperties{
                                .container.center =
                                    top_button_pos + next_button_offset * 2.0f,
                                .container.dimension = button_dimension,
                                .container.background_color = button_background_color,
                                .container.border.color = button_border_color,
                                .container.border.thickness = button_border_thickness,
                                .container.border.radius = button_border_radius,
                                .font.color = button_font_color,
                                .font.size = button_font_size})
                            .set_on_enter(on_enter_callback)
                            .set_on_leave(on_leave_callback)
                            .set_on_click(on_click_callback))

                        .add_button(
                            ui::Button(
                                "SETTINGS1",
                                ui::ElementProperties{
                                    .container.center =
                                        top_button_pos + next_button_offset * 0.0f,
                                    .container.dimension = button_dimension,
                                    .container.background_color = button_background_color,
                                    .container.border.color = button_border_color,
                                    .container.border.thickness = button_border_thickness,
                                    .container.border.radius = button_border_radius,
                                    .font.color = button_font_color,
                                    .font.size = button_font_size})
                                .set_on_enter(on_enter_callback)
                                .set_on_leave(on_leave_callback)
                                .set_on_click(on_click_callback))

                        .add_button(
                            ui::Button(
                                "SETTINGS2",
                                ui::ElementProperties{
                                    .container.center =
                                        top_button_pos + next_button_offset * 1.0f,
                                    .container.dimension = button_dimension,
                                    .container.background_color = button_background_color,
                                    .container.border.color = button_border_color,
                                    .container.border.thickness = button_border_thickness,
                                    .container.border.radius = button_border_radius,
                                    .font.color = button_font_color,
                                    .font.size = button_font_size})
                                .set_on_enter(on_enter_callback)
                                .set_on_leave(on_leave_callback)
                                .set_on_click(on_click_callback)))
                .add_button(
                    ui::Button("EXIT",
                               ui::ElementProperties{
                                   .container.center =
                                       top_button_pos + next_button_offset * 2.0f,
                                   .container.dimension = button_dimension,
                                   .container.background_color = button_background_color,
                                   .container.border.color = button_border_color,
                                   .container.border.thickness = button_border_thickness,
                                   .container.border.radius = button_border_radius,
                                   .font.color = button_font_color,
                                   .font.size = button_font_size})
                        .set_on_enter(on_enter_callback)
                        .set_on_leave(on_leave_callback)
                        .set_on_click(on_click_callback)));

        m_ui.add_text_box(
            NUMBER_ID,
            ui::TextBox(std::to_string(m_number),
                        ui::ElementProperties{.container.center = glm::vec2(0.0f, -250),
                                              .font.size = 158}));

        m_ui.add_text_box(VERSION_ID,
                          ui::TextBox("VERSION 1.234",
                                      ui::ElementProperties{
                                          .container.center = glm::vec2(-330.0f, 388.0f),
                                          .font.size = 24}));
    };

    ~UserInterfaceExample() {};

    void update(float dt) override {};

    void render(RenderEngine &render_engine) override {
        bool success = render_engine.begin_render_pass();
        if (!success) {
            return;
        }

        auto ui_state = m_ui.get_state();

        render_engine.render_ui(ui_state);

        success = render_engine.end_render_pass();
        if (!success) {
            return;
        }
    };

    void setup(RenderEngine &render_engine) override {
        render_engine.register_mouse_event_callback(
            [this](MouseEvent e, ViewportPoint &p) {
                this->m_ui.update_state_from_mouse_event(e, p);
            });
    }
};

int main() {

    GameEngineConfig config{
        .window_config =
            WindowConfig{.dims = WindowDimensions(800, 800), .title = "4_user_interface"},
        .use_font = UseFont::Default,
    };

    auto game = std::make_unique<UserInterfaceExample>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
