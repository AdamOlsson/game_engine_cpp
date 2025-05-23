#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/Window.h"
#include "render_engine/util.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>

class SwapChain {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;
    size_t m_next_frame_buffer;

    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_image_views;
    std::vector<VkFramebuffer> m_frame_buffers;

    uint32_t get_image_count(SwapChainSupportDetails &swap_chain_support);
    VkSurfaceFormatKHR
    choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &available_formats);
    VkPresentModeKHR choose_swap_present_mode(
        const std::vector<VkPresentModeKHR> &available_present_modes);
    VkExtent2D choose_swap_extent(GLFWwindow &window,
                                  const VkSurfaceCapabilitiesKHR &capabilities);
    VkSwapchainKHR create_swap_chain(uint32_t image_count,
                                     VkSurfaceFormatKHR &surface_format,
                                     SwapChainSupportDetails &swap_chain_support);

    std::vector<VkImage> create_swap_chain_images(uint32_t image_count);
    std::vector<VkImageView> create_image_views(VkFormat &image_format);
    std::vector<VkFramebuffer> create_frame_buffers();
    VkRenderPass create_render_pass(VkFormat &image_format);

  public:
    VkSwapchainKHR m_swap_chain;
    VkExtent2D m_extent;
    VkRenderPass m_render_pass;

    SwapChain() = default;
    SwapChain(std::shared_ptr<CoreGraphicsContext> ctx, const Window &window);
    ~SwapChain();

    SwapChain(const SwapChain &other) = delete;
    SwapChain(SwapChain &&other) noexcept = default;

    SwapChain &operator=(const SwapChain &other) = delete;
    SwapChain &operator=(SwapChain &&other) noexcept = default;

    VkFramebuffer get_frame_buffer();
    std::optional<uint32_t> get_next_image_index(VkSemaphore &image_available);
};
