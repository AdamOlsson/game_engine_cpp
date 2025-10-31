#pragma once

#include "game_engine_sdk/render_engine/vulkan/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <cstddef>
#include <memory>

namespace vulkan {
class Semaphore {
  private:
    std::shared_ptr<vulkan::GraphicsContext> m_ctx;
    size_t m_size;

    size_t m_next;
    std::vector<VkSemaphore> m_semaphores;

  public:
    Semaphore() = default;
    Semaphore(std::shared_ptr<vulkan::GraphicsContext> ctx, const size_t size);
    ~Semaphore();

    Semaphore(Semaphore &&other) noexcept = default;
    Semaphore &operator=(Semaphore &&other) noexcept = default;

    Semaphore(const Semaphore &other) = delete;
    Semaphore &operator=(const Semaphore &other) = delete;

    const VkSemaphore get();
};
} // namespace vulkan
