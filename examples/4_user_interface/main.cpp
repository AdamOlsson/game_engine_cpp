#include "Coordinates.h"
#include "Game.h"
#include "GameEngine.h"
#include "render_engine/WindowConfig.h"
#include "render_engine/colors.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/ui/Animation.h"
#include "render_engine/ui/AnimationCurve.h"
#include "render_engine/ui/UI.h"
#include <memory>

class UserInterfaceExample : public Game {
  private:
    ui::UI m_ui;
    ViewportPoint m_cursor_position;

    ui::Animation<glm::vec2> m_position_animation;

  public:
    UserInterfaceExample() : m_cursor_position(ViewportPoint(-10.0f, -10.0f)) {
        ui::ElementProperties element;
        element.center = glm::vec2(0.0, 100.0);
        element.dimension = glm::vec2(600.0, 200.0);
        element.border.color = colors::GREEN;
        element.border.thickness = 10.0;
        element.border.radius = 30.0;

        m_ui = ui::UI(element);

        m_position_animation =
            ui::AnimationBuilder()
                .set_on_completed(ui::OnAnimationCompleted::STOP)
                .set_animation_curve(ui::AnimationCurve::cos)
                .set_duration(300)
                .build(&m_ui.m_element.center, glm::vec2(0.0f, -100.0f));
    };

    ~UserInterfaceExample() {};

    void update(float dt) override {};

    void render(RenderEngine &render_engine) override {
        bool success = render_engine.begin_render_pass();
        if (!success) {
            return;
        }

        auto ui_state = m_ui.update_state_using_cursor(m_cursor_position);

        m_position_animation.increment();
        std::cout << std::endl;

        render_engine.render_ui(ui_state);

        success = render_engine.end_render_pass();
        if (!success) {
            return;
        }
    };

    void setup(RenderEngine &render_engine) override {
        render_engine.register_mouse_event_callback(
            [this](MouseEvent e, ViewportPoint &p) { this->m_cursor_position = p; });
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
