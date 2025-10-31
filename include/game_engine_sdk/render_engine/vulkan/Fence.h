#pragma once

#include "context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <cstddef>
#include <memory>

namespace vulkan {
class Fence {
  private:
    std::shared_ptr<context::GraphicsContext> m_ctx;
    size_t m_size;

    size_t m_current;
    size_t m_next;
    std::vector<VkFence> m_fences;

  public:
    Fence() = default;
    Fence(std::shared_ptr<context::GraphicsContext> ctx, const size_t size);
    ~Fence();

    Fence(Fence &&other) noexcept = default;
    Fence &operator=(Fence &&other) noexcept = default;

    Fence(const Fence &other) = delete;
    Fence &operator=(const Fence &other) = delete;

    const VkFence &next();
    const VkFence &current();
};

} // namespace vulkan
