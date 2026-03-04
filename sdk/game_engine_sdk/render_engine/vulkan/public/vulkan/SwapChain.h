#pragma once

#include "Framebuffer.h"
#include "ImageView.h"
#include "SwapChainImage.h"
#include "context/GraphicsContext.h"
#include "vulkan/Fence.h"
#include "vulkan/Frame.h"
#include "vulkan/Semaphore.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>

namespace vulkan {

class Frame;

class SwapChain {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;
    VkSwapchainKHR m_swap_chain;

    std::vector<vulkan::SwapChainImage> m_images;
    std::vector<vulkan::ImageView> m_image_views;
    std::vector<vulkan::Framebuffer> m_framebuffers;

    vulkan::Fence m_in_flight_fence;
    vulkan::Semaphore m_image_available;
    vulkan::Semaphore m_submit_completed;

    VkFormat m_surface_format;

    void setup(VkSwapchainKHR &old_swap_chain);

    VkSwapchainKHR create_swap_chain(
        uint32_t image_count, VkSurfaceFormatKHR &surface_format,
        vulkan::context::device::SwapChainSupportDetails &swap_chain_support,
        VkSwapchainKHR &old_swap_chain);

    std::vector<vulkan::SwapChainImage> create_swap_chain_images(uint32_t image_count);
    std::vector<vulkan::ImageView> create_image_views(VkFormat &image_format);

    void destroy();

  public:
    VkExtent2D m_extent;
    VkRenderPass m_render_pass;

    SwapChain();
    SwapChain(std::shared_ptr<vulkan::context::GraphicsContext> ctx);
    SwapChain(std::shared_ptr<vulkan::context::GraphicsContext> ctx, SwapChain &old);
    ~SwapChain();

    SwapChain(SwapChain &&other) noexcept = delete;
    SwapChain &operator=(SwapChain &&other) noexcept = delete;

    SwapChain(const SwapChain &other) = delete;
    SwapChain &operator=(const SwapChain &other) = delete;

    void wait_for_in_flight_fence();

    void recreate_swap_chain();

    std::vector<vulkan::Framebuffer> create_framebuffers(VkRenderPass &render_pass);
    VkRenderPass create_render_pass(VkFormat &image_format); // TODO: Deprecate

    VkRenderPass create_render_pass();

    VkFramebuffer get_framebuffer(uint32_t image_index);
    std::optional<uint32_t> get_next_image_index(VkSemaphore &image_available);

    vulkan::Frame begin_frame(vulkan::CommandBuffer &command_buffer);
    void set_image_index(vulkan::Frame &render_pass);
};
} // namespace vulkan
