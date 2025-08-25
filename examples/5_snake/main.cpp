#include "Game.h"
#include "GameEngine.h"

class Snake : public Game {
  private:
  public:
    Snake() {};
    ~Snake() {};

    void update(float dt) override {};

    void render() override {};

    void setup(std::shared_ptr<graphics_context::GraphicsContext> &ctx) override {}
};

int main() {

    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(800, 800),
                                              .title = "Snake"}};

    auto game = std::make_unique<Snake>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
