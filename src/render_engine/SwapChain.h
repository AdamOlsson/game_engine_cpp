#pragma once

#include "render_engine/Window.h"
#include "vulkan/vulkan_core.h"
#include <vector>

class SwapChain {
  public:
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;

    SwapChain(VkPhysicalDevice &physicalDevice, VkDevice &device, VkSurfaceKHR &surface,
              GLFWwindow &window);

    ~SwapChain();

    // TODO: Make Framebuffer its own class
    std::vector<VkFramebuffer> createFramebuffers(VkRenderPass &renderPass);

    void cleanup();

  private:
    // TODO: Shared ptr
    VkDevice *device;
    bool cleanup_done;
};
