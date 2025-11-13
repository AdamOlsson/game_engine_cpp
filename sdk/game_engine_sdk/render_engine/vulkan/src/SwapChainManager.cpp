#include "vulkan/SwapChainManager.h"
#include "vulkan/vulkan_core.h"
#include <optional>

namespace {
const int MAX_FRAMES_IN_FLIGHT = 2;
}

vulkan::SwapChainManager::SwapChainManager(
    std::shared_ptr<vulkan::context::GraphicsContext> ctx)
    : m_ctx(ctx), m_next_frame_buffer(0), m_swap_chain(SwapChain(ctx)),
      m_image_available(vulkan::Semaphore(m_ctx, MAX_FRAMES_IN_FLIGHT)),
      m_submit_completed(vulkan::Semaphore(m_ctx, MAX_FRAMES_IN_FLIGHT)),
      m_in_flight_fence(vulkan::Fence(m_ctx, MAX_FRAMES_IN_FLIGHT)),
      m_window_size_swap_buffer(
          vulkan::buffers::SwapUniformBuffer<window::WindowDimension<float>>(
              m_ctx, MAX_FRAMES_IN_FLIGHT, 1)) {
    m_window_size_swap_buffer.push_back(m_ctx->window->dimensions<float>());
    m_window_size_swap_buffer.transfer();
}

vulkan::SwapChainManager::~SwapChainManager() {}

void vulkan::SwapChainManager::recreate_swap_chain() {
    m_swap_chain.recreate_swap_chain();
}

void vulkan::SwapChainManager::wait_for_in_flight_fence() {
    const VkFence &in_flight_fence = m_in_flight_fence.next();
    m_ctx->logical_device.wait_for_fence(in_flight_fence);
    m_ctx->logical_device.reset_fence(in_flight_fence);
}

vulkan::RenderPass
vulkan::SwapChainManager::get_render_pass(vulkan::CommandBuffer &command_buffer) {
    wait_for_in_flight_fence();

    command_buffer.begin();
    auto render_pass = RenderPass(command_buffer, &m_swap_chain);

    set_image_index(render_pass);

    render_pass.m_render_pass = m_swap_chain.m_render_pass;
    render_pass.m_render_area_extent = m_swap_chain.m_extent;

    render_pass.m_frame_buffer = m_swap_chain.get_framebuffer(render_pass.m_image_index);
    render_pass.m_submit_completed = m_submit_completed.get();
    render_pass.m_device_queues = m_ctx->get_device_queues();

    vulkan::Viewport viewport{};
    viewport.width = m_swap_chain.m_extent.width;
    viewport.height = m_swap_chain.m_extent.height;
    render_pass.set_viewport(viewport);
    render_pass.set_scissor(m_swap_chain.m_extent);

    return render_pass;
}

void vulkan::SwapChainManager::set_image_index(RenderPass &render_pass) {

    VkSemaphore image_available = m_image_available.get();
    auto image_index = m_swap_chain.get_next_image_index(image_available);

    if (!image_index.has_value()) {
        logger::error("Failed to acquire image index");
        logger::info("Recreating swap chain");
        m_swap_chain.recreate_swap_chain();
        image_index = m_swap_chain.get_next_image_index(image_available);
    }

    VkFence in_flight_fence = m_in_flight_fence.current();
    render_pass.m_image_index = image_index.value();
    render_pass.m_image_available = image_available;
    render_pass.m_in_flight_fence = in_flight_fence;
}

std::vector<vulkan::buffers::GpuBufferRef>
vulkan::SwapChainManager::get_window_size_swap_buffer_ref() {
    return m_window_size_swap_buffer.get_buffer_references();
}
