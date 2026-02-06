#include "game_engine_sdk/GameEngine.h"
#include <memory>

GameEngine::GameEngine(std::unique_ptr<Game> game, GameEngineConfig &config)
    : m_start_tick(Clock::now()), m_next_tick(Duration::zero()),
      m_tick_delta(1.0 / config.ticks_per_second),
      m_window(std::make_unique<window::Window>(config.window_config)),
      m_ctx(std::make_shared<vulkan::context::GraphicsContext>(m_window.get())),
      m_game(std::move(game)) {}

GameEngine::~GameEngine() {}

void GameEngine::run() {

    m_game->setup(m_ctx);

    TimePoint log_timer = Clock::now();
    Duration log_frequency = std::chrono::seconds(5);
    size_t count = 0;

    while (!m_window->should_window_close()) {
        m_window->process_window_events();
        TimePoint current_time = Clock::now();
        Duration elapsed = current_time - m_start_tick;
        int update_count = 0;
        while (elapsed > m_next_tick && update_count < 5) {
            m_game->update(m_tick_delta.count());
            m_next_tick += m_tick_delta;
            update_count++;
        }
        count += update_count;

        // Log every 5 seconds
        Duration log_elapsed = current_time - log_timer;
        if (log_elapsed >= log_frequency) {
            std::cout << "Updates in last 5s: " << count << std::endl;
            count = 0;
            log_timer = current_time;
        }

        m_game->render();
    }
    m_ctx->wait_idle();
}
