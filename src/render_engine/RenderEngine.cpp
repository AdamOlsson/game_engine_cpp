#include "render_engine/RenderEngine.h"
#include "RenderBody.h"

void RenderEngine::render() {
    const std::vector<RenderBody> instance_data = {
        {glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0)}};
    this->g_ctx->render(this->window);
}

void RenderEngine::wait_idle() { this->g_ctx->waitIdle(); }

bool RenderEngine::should_window_close() { return this->window->should_window_close(); }

void RenderEngine::process_window_events() { this->window->process_window_events(); }

void RenderEngine::register_mouse_event_callback(std::function<void()> cb) {
    this->window->register_mouse_event_callback(cb);
}
