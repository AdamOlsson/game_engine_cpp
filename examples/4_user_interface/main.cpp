#include "Coordinates.h"
#include "Game.h"
#include "GameEngine.h"
#include "render_engine/WindowConfig.h"
#include "render_engine/colors.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/ui/ElementProperties.h"
#include "render_engine/ui/UI.h"
#include "render_engine/ui/animations/Animation.h"
#include "render_engine/ui/animations/AnimationCurve.h"
#include <memory>

void on_click_callback(ui::Button &self) { std::cout << "Click!" << std::endl; }
void on_enter_callback(ui::Button &self) {
    self.properties.container.border.color = colors::RED;
}
void on_leave_callback(ui::Button &self) {
    self.properties.container.border.color = colors::GREEN;
}

class UserInterfaceExample : public Game {
  private:
    std::unique_ptr<ui::UI> m_ui;

    ui::Animation<glm::vec2> m_position_animation;

  public:
    UserInterfaceExample() {

        // TODO: I am not sure I like the UI have to be a unique pointer. However, lets
        // see how things develop once we know how to create the layout
        m_ui = std::make_unique<ui::UI>(
            ui::Menu()
                .add_button(ui::Button(ui::ElementProperties{
                                           .container.center = glm::vec2(0.0, -250.0),
                                           .container.dimension = glm::vec2(400.0, 100.0),
                                           .container.border.color = colors::GREEN,
                                           .container.border.thickness = 5.0,
                                           .container.border.radius = 15.0,
                                       })
                                .set_on_enter(on_enter_callback)
                                .set_on_leave(on_leave_callback)
                                .set_on_click([](ui::Button &self) {
                                    self.animations.get<glm::vec2>("animation1").toggle();
                                })
                                .set_on_hover([](ui::Button &self) {})
                                .add_animation("animation1",
                                               [](ui::ElementProperties &props) {
                                                   return ui::AnimationBuilder()
                                                       .set_duration(300)
                                                       .set_animation_curve(
                                                           ui::AnimationCurve::smoothstep)
                                                       .set_on_completed(
                                                           ui::OnAnimationCompleted::LOOP)
                                                       .build(&props.container.center,
                                                              glm::vec2(100.0f, -250.0))
                                                       .play();
                                               }))
                .add_button(ui::Button(ui::ElementProperties{
                                           .container.center = glm::vec2(0.0, -140.0),
                                           .container.dimension = glm::vec2(400.0, 100.0),
                                           .container.border.color = colors::GREEN,
                                           .container.border.thickness = 5.0,
                                           .container.border.radius = 15.0,
                                       })
                                .set_on_enter(on_enter_callback)
                                .set_on_leave(on_leave_callback)
                                .set_on_click(on_click_callback)
                                .add_animation("color_animation1",
                                               [](ui::ElementProperties &props) {
                                                   return ui::AnimationBuilder()
                                                       .set_duration(600)
                                                       .set_on_completed(
                                                           ui::OnAnimationCompleted::STOP)
                                                       .set_animation_curve(
                                                           ui::AnimationCurve::linear)
                                                       .build(
                                                           &props.container.border.color,
                                                           colors::BLUE)
                                                       .play();
                                               }))
                .add_submenu(
                    ui::Menu(
                        ui::Button(ui::ElementProperties{
                                       .container.center = glm::vec2(0.0, -30.0),
                                       .container.dimension = glm::vec2(400.0, 100.0),
                                       .container.border.color = colors::BLUE,
                                       .container.border.thickness = 5.0,
                                       .container.border.radius = 15.0,
                                   })
                            .set_on_enter(on_enter_callback)
                            .set_on_leave(on_leave_callback)
                            .set_on_click(on_click_callback)
                            .add_animation("animation1",
                                           [](ui::ElementProperties &props) {
                                               return ui::AnimationBuilder()
                                                   .set_duration(300)
                                                   .set_animation_curve(
                                                       ui::AnimationCurve::smoothstep)
                                                   .set_on_completed(
                                                       ui::OnAnimationCompleted::REVERSE)
                                                   .build(&props.container.center,
                                                          glm::vec2(100.0f, -30.0f))
                                                   .play();
                                           }),
                        ui::Button(ui::ElementProperties{
                                       .container.center = glm::vec2(0.0, -30.0),
                                       .container.dimension = glm::vec2(400.0, 100.0),
                                       .container.border.color = colors::BLUE,
                                       .container.border.thickness = 5.0,
                                       .container.border.radius = 0.0,
                                   })
                            .set_on_enter(on_enter_callback)
                            .set_on_leave(on_leave_callback)
                            .set_on_click(on_click_callback))
                        .add_button(
                            ui::Button(ui::ElementProperties{
                                           .container.center = glm::vec2(0.0, -250.0),
                                           .container.dimension = glm::vec2(400.0, 100.0),
                                           .container.border.color = colors::GREEN,
                                           .container.border.thickness = 5.0,
                                           .container.border.radius = 0.0,
                                       })
                                .set_on_enter(on_enter_callback)
                                .set_on_leave(on_leave_callback)
                                .set_on_click(on_click_callback))
                        .add_button(
                            ui::Button(ui::ElementProperties{
                                           .container.center = glm::vec2(0.0, -140.0),
                                           .container.dimension = glm::vec2(400.0, 100.0),
                                           .container.border.color = colors::GREEN,
                                           .container.border.thickness = 5.0,
                                           .container.border.radius = 0.0,
                                       })
                                .set_on_enter(on_enter_callback)
                                .set_on_leave(on_leave_callback)
                                .set_on_click(on_click_callback))));
    };

    ~UserInterfaceExample() {};

    void update(float dt) override {};

    void render(RenderEngine &render_engine) override {
        bool success = render_engine.begin_render_pass();
        if (!success) {
            return;
        }

        auto ui_state = m_ui->get_state();

        render_engine.render_ui(ui_state);

        success = render_engine.end_render_pass();
        if (!success) {
            return;
        }
    };

    void setup(RenderEngine &render_engine) override {
        render_engine.register_mouse_event_callback(
            [this](MouseEvent e, ViewportPoint &p) {
                this->m_ui->update_state_from_mouse_event(e, p);
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
