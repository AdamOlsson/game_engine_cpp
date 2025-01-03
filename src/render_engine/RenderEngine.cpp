#include "render_engine/RenderEngine.h"
#include "render_engine/RenderBody.h"
#include "render_engine/buffers/StorageBuffer.h"
#include <memory>

void RenderEngine::render(std::vector<RenderBody> &bodies) {
    const std::vector<StorageBufferObject> ssbo = {
        StorageBufferObject(*bodies[0].position, *bodies[0].color, *bodies[0].rotation),
        StorageBufferObject(*bodies[1].position, *bodies[1].color, *bodies[1].rotation)};

    this->g_ctx->render(*window, ssbo);
}

void RenderEngine::wait_idle() { this->g_ctx->waitIdle(); }

bool RenderEngine::should_window_close() { return this->window->should_window_close(); }

void RenderEngine::process_window_events() { this->window->process_window_events(); }

void RenderEngine::register_mouse_event_callback(std::function<void()> cb) {
    this->window->register_mouse_event_callback(cb);
}
