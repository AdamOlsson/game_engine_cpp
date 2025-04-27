#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/Window.h"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <vector>

class SwapChain {
  private:
    std::shared_ptr<CoreGraphicsContext> ctx;
    bool cleanup_done;

  public:
    VkSwapchainKHR swap_chain;
    std::vector<VkImage> images;
    VkFormat image_format;
    VkExtent2D extent;
    std::vector<VkImageView> image_views;

    SwapChain(std::shared_ptr<CoreGraphicsContext> ctx, const Window &window);
    SwapChain(SwapChain &&other) noexcept;
    SwapChain &operator=(SwapChain &&other) noexcept;

    SwapChain(const SwapChain &other) = delete;
    SwapChain &operator=(const SwapChain &other) = delete;

    ~SwapChain();
};
