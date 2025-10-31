#pragma once

#include "CommandBuffer.h"
#include "Fence.h"
#include "RenderPass.h"
#include "Semaphore.h"
#include "SwapChain.h"
#include "buffers/GpuBuffer.h"
#include "context/GraphicsContext.h"
#include <memory>

namespace vulkan {
class SwapChainManager {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    size_t m_next_frame_buffer;

    vulkan::Fence m_in_flight_fence;
    vulkan::Semaphore m_image_available;
    vulkan::Semaphore m_submit_completed;

    vulkan::buffers::SwapUniformBuffer<window::WindowDimension<float>>
        m_window_size_swap_buffer;

  public:
    SwapChain m_swap_chain;

    SwapChainManager() = default;
    SwapChainManager(std::shared_ptr<vulkan::context::GraphicsContext> ctx);

    SwapChainManager(SwapChainManager &&other) noexcept = default;
    SwapChainManager &operator=(SwapChainManager &&other) noexcept = default;

    SwapChainManager(const SwapChainManager &other) = delete;
    SwapChainManager &operator=(const SwapChainManager &other) = delete;

    ~SwapChainManager();

    void recreate_swap_chain();

    void wait_for_in_flight_fence();
    RenderPass get_render_pass(vulkan::CommandBuffer &command_buffer);

    void set_image_index(RenderPass &render_pass);

    std::vector<vulkan::buffers::GpuBufferRef> get_window_size_swap_buffer_ref();
};
} // namespace vulkan
