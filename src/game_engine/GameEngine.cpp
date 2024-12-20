#include "GameEngine.h"

void GameEngine::run() {
    while (!this->render_engine->should_window_close()) {
        this->render_engine->process_window_events();
        // TODO: Update physics
        // TODO: Render physics
    }
}
