#pragma once

#include "render_engine/ImageView.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>

class SwapChain {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;
    size_t m_next_frame_buffer;

    std::vector<VkImage> m_images;
    std::vector<ImageView> m_image_views;
    std::vector<VkFramebuffer> m_frame_buffers;

    VkSwapchainKHR create_swap_chain(
        uint32_t image_count, VkSurfaceFormatKHR &surface_format,
        graphics_context::device::SwapChainSupportDetails &swap_chain_support);

    std::vector<VkImage> create_swap_chain_images(uint32_t image_count);
    std::vector<ImageView> create_image_views(VkFormat &image_format);
    std::vector<VkFramebuffer> create_framebuffers();
    VkRenderPass create_render_pass(VkFormat &image_format);

  public:
    VkSwapchainKHR m_swap_chain;
    VkExtent2D m_extent;
    VkRenderPass m_render_pass;

    SwapChain() = default;
    SwapChain(std::shared_ptr<graphics_context::GraphicsContext> ctx);
    ~SwapChain();

    SwapChain(const SwapChain &other) = delete;
    SwapChain(SwapChain &&other) noexcept = default;

    SwapChain &operator=(const SwapChain &other) = delete;
    SwapChain &operator=(SwapChain &&other) noexcept = default;

    VkFramebuffer get_frame_buffer();
    std::optional<uint32_t> get_next_image_index(VkSemaphore &image_available);
};
