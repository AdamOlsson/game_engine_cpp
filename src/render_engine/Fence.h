#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <cstddef>
#include <memory>

class Fence {
  private:
    std::shared_ptr<CoreGraphicsContext> ctx;
    size_t size;

    size_t next;
    std::vector<VkFence> fences;

  public:
    Fence() = default;
    Fence(std::shared_ptr<CoreGraphicsContext> ctx, const size_t size);
    ~Fence();

    Fence(const Fence &other) = delete;
    Fence(Fence &&other) noexcept = default;

    Fence &operator=(const Fence &other) = delete;
    Fence &operator=(Fence &&other) noexcept = default;

    const VkFence get();
};
