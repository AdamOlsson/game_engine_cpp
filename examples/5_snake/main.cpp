#include "Game.h"
#include "GameEngine.h"

class Snake : public Game {
  private:
  public:
    Snake() {};
    ~Snake() {};

    void update(float dt) override {};

    void render(RenderEngine &render_engine) override {};

    void setup(RenderEngine &render_engine) override {}
};

int main() {

    GameEngineConfig config{
        .window_config =
            WindowConfig{.dims = WindowDimensions(800, 800), .title = "Snake"},
        .use_font = UseFont::Default,
    };

    auto game = std::make_unique<Snake>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
