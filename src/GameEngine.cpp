#include "GameEngine.h"
#include "render_engine/RenderEngine.h"
#include <memory>

GameEngine::GameEngine(std::unique_ptr<Game> game, GameEngineConfig &config)
    : m_start_tick(Clock::now()), m_next_tick(Duration::zero()),
      m_tick_delta(1.0 / config.ticks_per_second), m_game(std::move(game)),
      m_render_engine(RenderEngine(config.window_config, config.use_font)) {}

GameEngine::~GameEngine() {}

void GameEngine::run() {
    m_game->setup(m_render_engine);

    while (!m_render_engine.should_window_close()) {

        m_render_engine.process_window_events();

        TimePoint current_time = Clock::now();
        Duration elapsed = current_time - m_start_tick;
        int update_count = 0;
        while (elapsed > m_next_tick && update_count < 5) {
            m_game->update(m_tick_delta.count());
            m_next_tick += m_tick_delta;
            update_count++;
        }

        m_game->render(m_render_engine);
    }

    m_render_engine.wait_idle();
}
