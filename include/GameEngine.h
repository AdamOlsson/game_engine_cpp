#pragma once

#include "render_engine/RenderEngine.h"
#include <cstdint>
#include <iostream>
#include <memory>

/**
 * @brief Represents a physics simulation and its visual representation.
 */
class GameEngine {
  private:
    std::unique_ptr<RenderEngine> render_engine;
    std::shared_ptr<uint32_t> click_count;

  public:
    GameEngine(std::unique_ptr<RenderEngine> render_engine)
        : render_engine(std::move(render_engine)),
          click_count(std::make_shared<uint32_t>(0)) {

        this->render_engine->register_mouse_event_callback(
            [this]() { this->increase_click_count(); });
    }

    ~GameEngine() {}

    /**
     * @brief Main loop of the game engine.
     */
    void run();

    // Temporary to test out event callbacks
    void increase_click_count() {
        (*this->click_count)++;
        std::cout << "Click count: " << *this->click_count << std::endl;
    }
};
