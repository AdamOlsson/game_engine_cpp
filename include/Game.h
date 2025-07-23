#pragma once

#include "render_engine/CoreGraphicsContext.h"
class Game {
  public:
    virtual ~Game() = default;
    virtual void update(const float dt) = 0;
    virtual void render() = 0;
    virtual void setup(std::shared_ptr<CoreGraphicsContext> &ctx) {};
};
