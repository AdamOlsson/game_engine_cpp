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
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;

    SwapChain(std::shared_ptr<CoreGraphicsContext> ctx, GLFWwindow &window);

    ~SwapChain();

    // TODO: Make Framebuffer its own class
    std::vector<VkFramebuffer> createFramebuffers(VkRenderPass &renderPass);

    void cleanup();
};
