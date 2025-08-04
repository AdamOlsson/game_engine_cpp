#pragma once

#include "render_engine/CommandBuffer.h"
#include "render_engine/Fence.h"
#include "render_engine/Semaphore.h"
#include "render_engine/SwapChain.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>

class SwapChainManager {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;

    size_t m_next_frame_buffer;

    Fence m_in_flight_fence;
    Semaphore m_image_available;
    Semaphore m_submit_completed;

    VkFence wait_for_in_flight_fence();

  public:
    SwapChain m_swap_chain;

    SwapChainManager() = default;
    SwapChainManager(std::shared_ptr<graphics_context::GraphicsContext> ctx);

    SwapChainManager(SwapChainManager &&other) noexcept = default;
    SwapChainManager &operator=(SwapChainManager &&other) noexcept = default;

    SwapChainManager(const SwapChainManager &other) = delete;
    SwapChainManager &operator=(const SwapChainManager &other) = delete;

    ~SwapChainManager();

    void recreate_swap_chain();

    void set_image_index(CommandBuffer &command_buffer);
};
