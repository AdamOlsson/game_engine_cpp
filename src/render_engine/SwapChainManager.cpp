#include "SwapChainManager.h"
#include "render_engine/graphics_pipeline/GeometryPipeline.h"
#include "vulkan/vulkan_core.h"
#include <optional>

SwapChainManager::SwapChainManager(std::shared_ptr<graphics_context::GraphicsContext> ctx)
    : m_ctx(ctx), m_next_frame_buffer(0), m_swap_chain(SwapChain(ctx)),
      m_image_available(
          vulkan::Semaphore(m_ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT)),
      m_submit_completed(
          vulkan::Semaphore(m_ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT)),
      m_in_flight_fence(vulkan::Fence(m_ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT)) {}

SwapChainManager::~SwapChainManager() {}

void SwapChainManager::recreate_swap_chain() {
    // All execution is paused when the window is minimized
    int width = 0, height = 0;
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(*m_ctx->window, &width, &height);
        glfwWaitEvents();
    }

    m_ctx->logical_device.wait_idle();
    m_swap_chain = SwapChain(m_ctx);
}

void SwapChainManager::wait_for_in_flight_fence() {
    const VkFence &in_flight_fence = m_in_flight_fence.next();
    m_ctx->logical_device.wait_for_fence(in_flight_fence);
    m_ctx->logical_device.reset_fence(in_flight_fence);
}

RenderPass SwapChainManager::get_render_pass(vulkan::CommandBuffer &command_buffer) {
    wait_for_in_flight_fence();

    command_buffer.begin();
    auto render_pass = RenderPass(command_buffer);

    render_pass.m_swap_chain = m_swap_chain.m_swap_chain;
    render_pass.m_render_pass = m_swap_chain.m_render_pass;
    render_pass.m_render_area_extent = m_swap_chain.m_extent;

    render_pass.m_frame_buffer = m_swap_chain.get_frame_buffer();
    render_pass.m_submit_completed = m_submit_completed.get();
    render_pass.m_device_queues = m_ctx->get_device_queues();

    render_pass.set_viewport(Dimension::from_extent2d(m_swap_chain.m_extent));
    render_pass.set_scissor(m_swap_chain.m_extent);

    set_image_index(render_pass);

    return render_pass;
}

void SwapChainManager::set_image_index(RenderPass &render_pass) {

    VkSemaphore image_available = m_image_available.get();
    auto image_index = m_swap_chain.get_next_image_index(image_available);

    if (!image_index.has_value()) {
        recreate_swap_chain();
        return;
    }

    VkFence in_flight_fence = m_in_flight_fence.current();
    render_pass.m_image_index = image_index.value();
    render_pass.m_image_available = image_available;
    render_pass.m_in_flight_fence = in_flight_fence;
}
