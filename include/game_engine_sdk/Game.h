#pragma once

#include "vulkan/context/GraphicsContext.h"
class Game {
  public:
    virtual ~Game() = default;
    virtual void update(const float dt) = 0;
    virtual void render() = 0;
    virtual void setup(std::shared_ptr<vulkan::context::GraphicsContext> &ctx) {};
};
