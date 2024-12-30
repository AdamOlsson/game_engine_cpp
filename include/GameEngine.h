#pragma once

#include "Game.h"
#include "render_engine/RenderEngine.h"
#include <memory>

struct GameEngineConfig {
    uint32_t window_width;
    uint32_t window_height;
    char const *window_title;
};

class GameEngine {
  private:
    std::unique_ptr<Game> game;
    std::unique_ptr<RenderEngine> render_engine;

    std::shared_ptr<uint32_t> click_count;

  public:
    GameEngine(std::unique_ptr<Game>, GameEngineConfig &);
    ~GameEngine();

    void run();
};
