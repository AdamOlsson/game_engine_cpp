#pragma once

#include "render_engine/CommandBuffer.h"
#include "render_engine/Fence.h"
#include "render_engine/RenderPass.h"
#include "render_engine/Semaphore.h"
#include "render_engine/SwapChain.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include <memory>

class SwapChainManager {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;

    size_t m_next_frame_buffer;

    Fence m_in_flight_fence;
    Semaphore m_image_available;
    Semaphore m_submit_completed;

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

    void wait_for_in_flight_fence();
    RenderPass get_render_pass(CommandBuffer &command_buffer);

    void set_image_index(RenderPass &render_pass);
};
