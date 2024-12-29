#include "render_engine/RenderEngine.h"
#include "RenderBody.h"
#include "render_engine/buffers/StorageBuffer.h"
#include <memory>

void RenderEngine::render() {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime -
                                                                            startTime)
                     .count();

    const std::vector<RenderBody> instance_data = {
        RenderBody(glm::vec3(0.5, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0),
                   (time / 5.0) * glm::radians(90.0)),
        RenderBody(glm::vec3(-0.5, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0),
                   (time / 5.0) * glm::radians(-90.0))};

    // TODO: Can probably avoid a write here by simply using StorageBufferObject or
    // RenderBody
    const std::vector<StorageBufferObject> ssbo = {
        StorageBufferObject(instance_data[0].position, instance_data[0].color,
                            instance_data[0].rotation),
        StorageBufferObject(instance_data[1].position, instance_data[1].color,
                            instance_data[1].rotation)};

    this->g_ctx->render(*window, ssbo);
}

void RenderEngine::wait_idle() { this->g_ctx->waitIdle(); }

bool RenderEngine::should_window_close() { return this->window->should_window_close(); }

void RenderEngine::process_window_events() { this->window->process_window_events(); }

void RenderEngine::register_mouse_event_callback(std::function<void()> cb) {
    this->window->register_mouse_event_callback(cb);
}
