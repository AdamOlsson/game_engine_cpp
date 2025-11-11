#pragma once

#include "Framebuffer.h"
#include "ImageView.h"
#include "SwapChainImage.h"
#include "context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>

namespace vulkan {

class SwapChain {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;
    size_t m_next_frame_buffer;

    std::vector<vulkan::SwapChainImage> m_images;
    std::vector<vulkan::ImageView> m_image_views;
    std::vector<vulkan::Framebuffer> m_framebuffers;

    void setup(VkSwapchainKHR &old_swap_chain);

    VkSwapchainKHR create_swap_chain(
        uint32_t image_count, VkSurfaceFormatKHR &surface_format,
        vulkan::context::device::SwapChainSupportDetails &swap_chain_support,
        VkSwapchainKHR &old_swap_chain);

    std::vector<vulkan::SwapChainImage> create_swap_chain_images(uint32_t image_count);
    std::vector<vulkan::ImageView> create_image_views(VkFormat &image_format);
    std::vector<vulkan::Framebuffer> create_framebuffers();
    VkRenderPass create_render_pass(VkFormat &image_format);

    void destroy();

  public:
    VkSwapchainKHR m_swap_chain;
    VkExtent2D m_extent;
    VkRenderPass m_render_pass;

    SwapChain();
    SwapChain(std::shared_ptr<vulkan::context::GraphicsContext> ctx);
    SwapChain(std::shared_ptr<vulkan::context::GraphicsContext> ctx, SwapChain &old);
    ~SwapChain();

    SwapChain(SwapChain &&other) noexcept;
    SwapChain &operator=(SwapChain &&other) noexcept;

    SwapChain(const SwapChain &other) = delete;
    SwapChain &operator=(const SwapChain &other) = delete;

    void recreate_swap_chain();

    VkFramebuffer get_framebuffer(uint32_t image_index);
    std::optional<uint32_t> get_next_image_index(VkSemaphore &image_available);
};
} // namespace vulkan
