#include "GameEngine.h"
#include "render_engine/RenderEngine.h"
#include <cstdint>
#include <memory>

int main() {
    uint32_t window_width = 800;
    uint32_t window_height = 600;
    char const *window_title = "Game Window";
    auto render_engine =
        std::make_unique<RenderEngine>(window_width, window_height, window_title);
    auto game_engine = std::make_unique<GameEngine>(std::move(render_engine));
    game_engine->run();
    return 0;
}
