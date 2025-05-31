#include "Coordinates.h"
#include "Game.h"
#include "GameEngine.h"
#include "render_engine/WindowConfig.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/ui/UI.h"
#include <memory>

class UserInterfaceExample : public Game {
  private:
    ui::UI m_ui;
    ViewportPoint m_cursor_position;

  public:
    UserInterfaceExample()
        : m_ui(ui::Layout()), m_cursor_position(ViewportPoint(-10.0f, -10.0f)) {};

    ~UserInterfaceExample() {};

    void update(float dt) override {};

    void render(RenderEngine &render_engine) override {
        bool success = render_engine.begin_render_pass();
        if (!success) {
            return;
        }

        /*std::cout << m_cursor_position << std::endl;*/
        auto ui_state = m_ui.update_state_using_cursor(m_cursor_position);
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
