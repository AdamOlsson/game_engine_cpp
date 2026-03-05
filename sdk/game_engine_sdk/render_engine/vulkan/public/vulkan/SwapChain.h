#pragma once

#include "Framebuffer.h"
#include "ImageView.h"
#include "SwapChainImage.h"
#include "context/GraphicsContext.h"
#include "vulkan/Extent2D.h"
#include "vulkan/Fence.h"
#include "vulkan/Frame.h"
#include "vulkan/RenderPass.h"
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

    vulkan::Format m_surface_format;
    vulkan::Extent2D m_extent;
    std::optional<vulkan::RenderPass> m_render_pass;

    void setup(VkSwapchainKHR &old_swap_chain);

    VkSwapchainKHR create_swap_chain(
        uint32_t image_count, VkSurfaceFormatKHR &surface_format,
        vulkan::context::device::SwapChainSupportDetails &swap_chain_support,
        VkSwapchainKHR &old_swap_chain);

    std::vector<vulkan::SwapChainImage> create_swap_chain_images(uint32_t image_count);
    std::vector<vulkan::ImageView> create_image_views(VkFormat &image_format);

    void destroy();

  public:
    SwapChain();
    SwapChain(std::shared_ptr<vulkan::context::GraphicsContext> &ctx);
    SwapChain(std::shared_ptr<vulkan::context::GraphicsContext> &ctx, SwapChain &old);
    ~SwapChain();

    SwapChain(SwapChain &&other) noexcept = delete;
    SwapChain &operator=(SwapChain &&other) noexcept = delete;

    SwapChain(const SwapChain &other) = delete;
    SwapChain &operator=(const SwapChain &other) = delete;

    vulkan::Extent2D get_extent() const { return m_extent; }
    vulkan::RenderPass *get_render_pass_handle() {
        return m_render_pass.has_value() ? &m_render_pass.value() : nullptr;
    }

    void wait_for_in_flight_fence();

    void recreate_swap_chain();

    void create_framebuffers(vulkan::RenderPass &render_pass);
    vulkan::RenderPass
    create_render_pass(const std::optional<RenderPassOpts> opts = std::nullopt);

    std::optional<uint32_t> get_next_image_index(VkSemaphore &image_available);

    vulkan::Frame begin_frame(vulkan::CommandBuffer &command_buffer,
                              vulkan::RenderPass *render_pass = nullptr);
    void set_image_index(vulkan::Frame &render_pass);
};
} // namespace vulkan
