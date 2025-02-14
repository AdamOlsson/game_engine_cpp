#include "render_engine/RenderEngine.h"
#include "render_engine/RenderBody.h"
#include "render_engine/Window.h"
#include "render_engine/buffers/StorageBuffer.h"
#include <memory>

RenderEngine::RenderEngine(const uint32_t width, const uint32_t height, char const *title)
    : window(std::make_unique<Window>(width, height, title)),
      g_ctx(CoreGraphicsContext(true, window.get())) {
    graphics_pipeline = std::make_unique<GraphicsPipeline>(*window, g_ctx);
}

RenderEngine::~RenderEngine() { delete graphics_pipeline.release(); }

void RenderEngine::render(
    const std::vector<std::reference_wrapper<const RenderBody>> &bodies) {

    std::vector<StorageBufferObject> triangle_instance_data = {};
    std::vector<StorageBufferObject> rectangle_instance_data = {};
    for (auto b : bodies) {
        auto deref_b = b.get();
        switch (deref_b.shape.encode_shape_type()) {
        case ShapeTypeEncoding::TriangleShape:
            triangle_instance_data.push_back(StorageBufferObject(
                deref_b.position, deref_b.color, deref_b.rotation, deref_b.shape));
            break;
        case ShapeTypeEncoding::RectangleShape:
            rectangle_instance_data.push_back(StorageBufferObject(
                deref_b.position, deref_b.color, deref_b.rotation, deref_b.shape));
            break;
        default:
            break;
        };
    }

    this->graphics_pipeline->render(*window, triangle_instance_data,
                                    rectangle_instance_data);
}

void RenderEngine::wait_idle() { this->g_ctx.wait_idle(); }

bool RenderEngine::should_window_close() { return this->window->should_window_close(); }

void RenderEngine::process_window_events() { this->window->process_window_events(); }

void RenderEngine::register_mouse_event_callback(MouseEventCallbackFn cb) {
    this->window->register_mouse_event_callback(cb);
}

void RenderEngine::register_keyboard_event_callback(KeyboardEventCallbackFn cb) {
    this->window->register_keyboard_event_callback(cb);
}
