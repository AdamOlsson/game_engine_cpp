#pragma once

#include "GameState.h"
#include <memory>

// forward declarations
namespace vulkan::context {
class GraphicsContext;
}
class CaravanDefence;

class Configuration {
  public:
    static void
    setup_world_renderers(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                          CaravanDefence &game);

    static void setup_ui_renderers(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                                   CaravanDefence &game);

    static void
    setup_mouse_event_handler(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                              CaravanDefence &game);

    static void
    setup_keyboard_event_handler(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                                 CaravanDefence &game);

    static void
    setup_initial_game_state(graphics_pipeline::geometry::GeometryRenderer *geom_renderer,
                             graphics_pipeline::quad::QuadRenderer *quad_renderer,
                             GameState &game_state);
};
