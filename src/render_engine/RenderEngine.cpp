#include "render_engine/RenderEngine.h"
#include "render_engine/RenderBody.h"
#include "render_engine/buffers/StorageBuffer.h"
#include <memory>

void RenderEngine::render(std::vector<RenderBody> &bodies) {

    std::vector<StorageBufferObject> ssbo = {};
    for (auto b : bodies) {
        ssbo.push_back(StorageBufferObject(b.position, b.color, b.rotation,
                                           b.shape.param.triangle.side));
    }

    this->g_ctx->render(*window, ssbo);
}

void RenderEngine::wait_idle() { this->g_ctx->waitIdle(); }

bool RenderEngine::should_window_close() { return this->window->should_window_close(); }

void RenderEngine::process_window_events() { this->window->process_window_events(); }

void RenderEngine::register_mouse_event_callback(MouseEventCallbackFn cb) {
    this->window->register_mouse_event_callback(cb);
}
