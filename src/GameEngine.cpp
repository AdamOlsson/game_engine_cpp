#include "GameEngine.h"
#include "render_engine/RenderEngine.h"
#include <memory>

GameEngine::GameEngine(std::unique_ptr<Game> game, GameEngineConfig &config)
    : game(std::move(game)), start_tick(Clock::now()), next_tick(Duration::zero()),
      tick_delta(1.0 / config.ticks_per_second) {

    render_engine = std::make_unique<RenderEngine>(
        config.window_width, config.window_height, config.window_title);
}

GameEngine::~GameEngine() {}

void GameEngine::run() {
    this->game->setup(*render_engine);

    while (!this->render_engine->should_window_close()) {

        this->render_engine->process_window_events();

        TimePoint current_time = Clock::now();
        Duration elapsed = current_time - start_tick;
        int update_count = 0;
        while (elapsed > next_tick && update_count < 5) {
            this->game->update(tick_delta.count());
            next_tick += tick_delta;
            update_count++;
        }

        this->game->render(*render_engine);
    }

    this->render_engine->wait_idle();
}
