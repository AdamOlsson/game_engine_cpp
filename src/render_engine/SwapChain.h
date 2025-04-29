#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/Window.h"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <vector>

class SwapChain {
  private:
    const Window *m_window;
    std::shared_ptr<CoreGraphicsContext> m_ctx;

    size_t m_next_frame_buffer;
    std::vector<VkImage> m_images;

    std::vector<VkFramebuffer> m_frame_buffer;

    VkSurfaceFormatKHR
    choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &available_formats);
    VkPresentModeKHR choose_swap_present_mode(
        const std::vector<VkPresentModeKHR> &available_present_modes);
    VkExtent2D choose_swap_extent(GLFWwindow &window,
                                  const VkSurfaceCapabilitiesKHR &capabilities);
    uint32_t get_image_count();

    std::vector<VkImage> create_swap_chain_images(uint32_t image_count);
    std::vector<VkImageView> create_image_views();

    VkRenderPass create_render_pass();
    std::vector<VkFramebuffer> create_frame_buffers();

  public:
    VkSwapchainKHR m_swap_chain;
    VkFormat m_image_format;
    VkExtent2D m_extent;
    std::vector<VkImageView> image_views;
    VkRenderPass m_render_pass;

    SwapChain(std::shared_ptr<CoreGraphicsContext> ctx, const Window &window);
    // TODO: UPDATE THESE AFTER REFACTORING
    SwapChain(SwapChain &&other) noexcept;
    SwapChain(const SwapChain &other) = delete;
    SwapChain &operator=(SwapChain &&other) noexcept;
    SwapChain &operator=(const SwapChain &other) = delete;

    ~SwapChain();

    VkFramebuffer get_frame_buffer();
    /*void recreate_swap_chain();*/
    std::optional<int> begin_render_pass(VkCommandBuffer &command_buffer);
};
