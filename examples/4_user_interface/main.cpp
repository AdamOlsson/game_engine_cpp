#include "Coordinates.h"
#include "Game.h"
#include "GameEngine.h"
#include "render_engine/WindowConfig.h"
#include "render_engine/colors.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/ui/Animation.h"
#include "render_engine/ui/AnimationCurve.h"
#include "render_engine/ui/ElementProperties.h"
#include "render_engine/ui/UI.h"
#include <memory>

class UserInterfaceExample : public Game {
  private:
    std::unique_ptr<ui::UI> m_ui;
    ViewportPoint m_cursor_position;

    ui::Animation<glm::vec2> m_position_animation;

  public:
    UserInterfaceExample() : m_cursor_position(ViewportPoint(-10.0f, -10.0f)) {
        auto element1 = std::make_unique<ui::ElementProperties>();
        element1->center = glm::vec2(0.0, -150.0);
        element1->dimension = glm::vec2(600.0, 200.0);
        element1->border.color = colors::GREEN;
        element1->border.thickness = 10.0;
        element1->border.radius = 30.0;

        auto element2 = std::make_unique<ui::ElementProperties>();
        element2->center = glm::vec2(0.0, 150.0);
        element2->dimension = glm::vec2(600.0, 200.0);
        element2->border.color = colors::GREEN;
        element2->border.thickness = 10.0;
        element2->border.radius = 30.0;

        m_position_animation = ui::AnimationBuilder()
                                   .set_on_completed(ui::OnAnimationCompleted::STOP)
                                   .set_animation_curve(ui::AnimationCurve::cos)
                                   .set_duration(300)
                                   .build(&element2->center, glm::vec2(50.0f, 150.0f));

        std::vector<std::unique_ptr<ui::ElementProperties>> elements;
        elements.push_back(std::move(element1));
        elements.push_back(std::move(element2));

        // TODO: I am not sure I like the UI have to be a unique pointer. However, lets
        // see how things develop once we know how to create the layout
        m_ui = std::make_unique<ui::UI>(std::move(elements));
    };

    ~UserInterfaceExample() {};

    void update(float dt) override {};

    void render(RenderEngine &render_engine) override {
        bool success = render_engine.begin_render_pass();
        if (!success) {
            return;
        }

        auto ui_state = m_ui->update_state_using_cursor(m_cursor_position);

        m_position_animation.increment();

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
