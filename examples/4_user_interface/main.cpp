#include "Game.h"
#include "GameEngine.h"
#include "render_engine/WindowConfig.h"
#include "render_engine/fonts/Font.h"
#include <memory>

class UserInterfaceExample : public Game {
  public:
    UserInterfaceExample() {};

    ~UserInterfaceExample() {};

    void update(float dt) override {};

    void render(RenderEngine &render_engine) override {
        bool success = render_engine.begin_render_pass();
        if (!success) {
            return;
        }

        ui::ElementProperties ui_element{};
        ui_element.center = glm::vec2(0.0, 100.0);
        ui_element.dimension = glm::vec2(600.0, 200.0);
        ui_element.border.thickness = 20.0;
        ui_element.border.radius = 30.0;

        render_engine.render_ui(ui_element);

        success = render_engine.end_render_pass();
        if (!success) {
            return;
        }
    };

    void setup(RenderEngine &render_engine) override {}
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
