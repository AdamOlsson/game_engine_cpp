#include "GameEngine.h"
#include "render_engine/RenderEngine.h"
#include <cstdint>
#include <memory>

// TODO: Why is not my StorageBuffer data read?
// - Perhaps I need to use some instance index to access the data?
// - Look at some Youtube video that implements Storage Buffers
// TODO: Implement Instanced rendering for GraphicsContext (triangles)
// TODO: Implement RigidBody (triangle)
// TODO: Implement click and move triangles
// TODO: Implement SAT
// TODO: Implement SAT example

int main() {
    uint32_t window_width = 800;
    uint32_t window_height = 800;
    char const *window_title = "Game Window";
    auto render_engine =
        std::make_unique<RenderEngine>(window_width, window_height, window_title);
    auto game_engine = std::make_unique<GameEngine>(std::move(render_engine));
    game_engine->run();
    auto window = std::make_unique<Window>(window_width, window_height, window_title);

    return 0;
}
