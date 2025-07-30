#include "Game.h"
#include "GameEngine.h"

class Snake : public Game {
  private:
  public:
    Snake() {
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
        /*std::make_unique<GeometryPipeline>(*/
        /*       ctx, m_swap_chain_manager, m_window_dimension_buffers, m_sampler,
         *m_texture);*/

    };
    ~Snake() {};

    void update(float dt) override {};

    void render() override {};

    void setup(std::shared_ptr<graphics_context::GraphicsContext> &ctx) override {}
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
