#pragma once
#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/GraphicsPipeline.h"
#include "render_engine/RenderBody.h"
#include "render_engine/Window.h"
#include <GLFW/glfw3.h>

class RenderEngine {
  public:
    RenderEngine(const uint32_t width, const uint32_t height, char const *title);
    ~RenderEngine();

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
    void register_mouse_event_callback(MouseEventCallbackFn);

    void render(std::vector<RenderBody> &);
    void wait_idle();

  private:
    std::unique_ptr<Window> window;
    CoreGraphicsContext g_ctx;
    std::unique_ptr<GraphicsPipeline> graphics_pipeline;
};
