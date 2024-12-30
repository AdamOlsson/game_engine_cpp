#include "GameEngine.h"
#include "render_engine/RenderEngine.h"
#include <memory>

GameEngine::GameEngine(std::unique_ptr<Game> game, GameEngineConfig &config)
    : game(std::move(game)), click_count(std::make_shared<uint32_t>(0)) {

    render_engine = std::make_unique<RenderEngine>(
        config.window_width, config.window_height, config.window_title);
}

GameEngine::~GameEngine() {}

void GameEngine::run() {
    this->game->setup(*render_engine);

    while (!this->render_engine->should_window_close()) {
        this->render_engine->process_window_events();
        this->game->update();
        this->game->render(*render_engine);
    }

    this->render_engine->wait_idle();
}
