#include "GameEngine.h"

void GameEngine::run() {
    while (!this->render_engine->should_window_close()) {
        this->render_engine->process_window_events();

        this->render_engine->render();
    }

    this->render_engine->wait_idle();
}
