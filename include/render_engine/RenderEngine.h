#pragma once
#include "render_engine/GraphicsContext.h"
#include "render_engine/Window.h"
#include <GLFW/glfw3.h>
#include <functional>

class RenderEngine {
  public:
    RenderEngine(const uint32_t width, const uint32_t height, char const *title) {

        window = std::make_unique<Window>(width, height, title);
        g_ctx = std::make_unique<GraphicsContext>(*window);
    }

    ~RenderEngine() {}

    bool should_window_close();

    /**
     * @brief Processes any events GLFW has queued. This function triggers any callback
     * registered. Currently only forwards the call to the Window class member.
     */
    void process_window_events();

    /**
     * @brief Register a callback function for mouse input events. Currently only forwards
     * the call to the Window class member.
     * @param cb The callback function which is triggered after a mouse input event.
     */
    void register_mouse_event_callback(std::function<void()>);

    void render();
    void wait_idle();

  private:
    std::unique_ptr<Window> window;
    std::unique_ptr<GraphicsContext> g_ctx;
};
