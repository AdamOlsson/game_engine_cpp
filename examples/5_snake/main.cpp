#include "Game.h"
#include "GameEngine.h"

class Snake : public Game {
  private:
  public:
    Snake() {
        /*auto ctx = std::make_shared<CoreGraphicsContext>(window);*/
        /*auto swap_chain_manager = SwapChainManager(ctx, window);*/
        // create and configure text renderer
        /*
         * // Mandatory args
         * - shaders
         * - Window()
         * - SwapChainManager()
         * - CoreGraphicsContext()
         * auto builder = TextRendererBuilder();
         * // Generic
         * builder.add_buffer(< uniform buffer >);
         * builder.add_buffer(< storage buffer >);
         * builder.add_texture(< texture >);
         *
         * // Specific
         * builder.add_font(< font >); // includes kerning
         *
         */
    };
    ~Snake() {};

    void update(float dt) override {};

    void render(RenderEngine &render_engine) override {};

    void setup(window::Window *) override {}
};

int main() {

    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(800, 800),
                                              .title = "Snake"},
        .use_font = UseFont::Default,
    };

    auto game = std::make_unique<Snake>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
