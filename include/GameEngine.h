#pragma once

#include "Game.h"
#include "render_engine/RenderEngine.h"
#include <memory>

struct GameEngineConfig {
    uint32_t window_width;
    uint32_t window_height;
    char const *window_title;
    float ticks_per_second = 60.0f;
};

class GameEngine {
  private:
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<double>;

    TimePoint start_tick;
    Duration next_tick;
    Duration tick_delta;

    std::unique_ptr<Game> game;
    std::unique_ptr<RenderEngine> render_engine;

  public:
    GameEngine(std::unique_ptr<Game>, GameEngineConfig &);
    ~GameEngine();

    void run();
};
