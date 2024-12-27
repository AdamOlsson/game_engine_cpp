#include "render_engine/RenderEngine.h"

void RenderEngine::render() { this->g_ctx->render(this->window); }

void RenderEngine::wait_idle() { this->g_ctx->waitIdle(); }

bool RenderEngine::should_window_close() { return this->window->should_window_close(); }

void RenderEngine::process_window_events() { this->window->process_window_events(); }

void RenderEngine::register_mouse_event_callback(std::function<void()> cb) {
    this->window->register_mouse_event_callback(cb);
}
