#pragma once

#include "render_engine/CommandBufferManager.h"

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/Fence.h"
#include "render_engine/Semaphore.h"
#include "render_engine/SwapChain.h"
#include "render_engine/Window.h"
#include "vulkan/vulkan_core.h"
#include <memory>

class SwapChainManager {
  private:
    const Window *m_window;
    std::shared_ptr<CoreGraphicsContext> m_ctx;

    size_t m_next_frame_buffer;
    CommandBufferManager m_command_buffer_manager;

    Fence m_in_flight_fence;
    Semaphore m_image_available;
    Semaphore m_submit_completed;

    VkFence wait_for_in_flight_fence();

  public:
    SwapChain m_swap_chain;

    SwapChainManager(std::shared_ptr<CoreGraphicsContext> ctx, const Window &window);

    SwapChainManager(SwapChainManager &&other) noexcept = default;
    SwapChainManager(const SwapChainManager &other) = delete;
    SwapChainManager &operator=(SwapChainManager &&other) noexcept = default;
    SwapChainManager &operator=(const SwapChainManager &other) = delete;

    ~SwapChainManager();

    void recreate_swap_chain();

    SingleTimeCommandBuffer get_single_time_command_buffer();
    std::optional<CommandBuffer> get_command_buffer();
};
