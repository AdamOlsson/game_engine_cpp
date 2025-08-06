#pragma once

#include "render_engine/graphics_context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <cstddef>
#include <memory>

namespace vulkan {
class Semaphore {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;
    size_t m_size;

    size_t m_next;
    std::vector<VkSemaphore> m_semaphores;

  public:
    Semaphore() = default;
    Semaphore(std::shared_ptr<graphics_context::GraphicsContext> ctx, const size_t size);
    ~Semaphore();

    Semaphore(Semaphore &&other) noexcept = default;
    Semaphore &operator=(Semaphore &&other) noexcept = default;

    Semaphore(const Semaphore &other) = delete;
    Semaphore &operator=(const Semaphore &other) = delete;

    const VkSemaphore get();
};
} // namespace vulkan
