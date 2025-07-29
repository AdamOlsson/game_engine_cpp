#include "SwapChainManager.h"
#include "render_engine/GeometryPipeline.h"
#include "vulkan/vulkan_core.h"
#include <optional>

SwapChainManager::SwapChainManager(std::shared_ptr<CoreGraphicsContext> ctx)
    : m_ctx(ctx), m_next_frame_buffer(0), m_swap_chain(SwapChain(ctx)),
      m_command_buffer_manager(CommandBufferManager(m_ctx, MAX_FRAMES_IN_FLIGHT)),
      m_image_available(Semaphore(m_ctx, MAX_FRAMES_IN_FLIGHT)),
      m_submit_completed(Semaphore(m_ctx, MAX_FRAMES_IN_FLIGHT)),
      m_in_flight_fence(Fence(m_ctx, MAX_FRAMES_IN_FLIGHT)) {}

SwapChainManager::~SwapChainManager() {}

void SwapChainManager::recreate_swap_chain() {
    // All execution is paused when the window is minimized
    int width = 0, height = 0;
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(*m_ctx->window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_ctx->logical_device);
    m_swap_chain = SwapChain(m_ctx);
}

SingleTimeCommandBuffer SwapChainManager::get_single_time_command_buffer() {
    return m_command_buffer_manager.get_single_time_command_buffer();
}

VkFence SwapChainManager::wait_for_in_flight_fence() {
    const VkFence in_flight_fence = m_in_flight_fence.get();
    vkWaitForFences(m_ctx->logical_device, 1, &in_flight_fence, VK_TRUE, UINT64_MAX);

    vkResetFences(m_ctx->logical_device, 1, &in_flight_fence);
    return in_flight_fence;
}

std::optional<CommandBuffer> SwapChainManager::get_command_buffer() {

    VkFence in_flight_fence = wait_for_in_flight_fence();

    VkSemaphore image_available = m_image_available.get();
    auto image_index = m_swap_chain.get_next_image_index(image_available);

    if (!image_index.has_value()) {
        recreate_swap_chain();
        return std::nullopt;
    }

    auto command_buffer = m_command_buffer_manager.get_command_buffer();
    command_buffer.m_swap_chain = m_swap_chain.m_swap_chain;
    command_buffer.m_render_pass = m_swap_chain.m_render_pass;
    command_buffer.m_render_area_extent = m_swap_chain.m_extent;
    command_buffer.m_frame_buffer = m_swap_chain.get_frame_buffer();
    command_buffer.m_image_available = image_available;
    command_buffer.m_submit_completed = m_submit_completed.get();
    command_buffer.m_in_flight_fence = in_flight_fence;
    command_buffer.m_image_index = image_index.value();

    return command_buffer;
}
