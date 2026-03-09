#pragma once

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
};
