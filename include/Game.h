#pragma once

#include "render_engine/RenderEngine.h"
class Game {
  public:
    virtual ~Game() = default;
    virtual void update(const float dt) = 0;
    virtual void render(RenderEngine &render_engine) = 0;
    virtual void setup(RenderEngine &render_engine) = 0;
};
