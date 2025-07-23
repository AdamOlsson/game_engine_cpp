#pragma once

#include "Game.h"
#include "render_engine/window/WindowConfig.h"
#include <memory>

struct GameEngineConfig {
    window::WindowConfig window_config;
    float ticks_per_second = 60.0f;
    UseFont use_font = UseFont::None;
};

class GameEngine {
  private:
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<double>;

    TimePoint m_start_tick;
    Duration m_next_tick;
    Duration m_tick_delta;

    std::unique_ptr<window::Window> m_window;
    std::shared_ptr<CoreGraphicsContext> m_ctx;
    std::unique_ptr<Game> m_game;

    RenderEngine m_render_engine;

  public:
    GameEngine(std::unique_ptr<Game>, GameEngineConfig &);
    ~GameEngine();

    void run();
};
