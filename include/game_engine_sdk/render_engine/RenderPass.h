#pragma once

#include "game_engine_sdk/Dimension.h"
#include "game_engine_sdk/render_engine/SwapChain.h"
#include "game_engine_sdk/render_engine/vulkan/CommandBuffer.h"
#include "game_engine_sdk/render_engine/vulkan/GraphicsContext.h"
class RenderPass {
  private:
    vulkan::CommandBuffer m_command_buffer;
    SwapChain *m_swap_chain;

  public:
    VkRenderPass m_render_pass;
    VkExtent2D m_render_area_extent;
    VkFramebuffer m_frame_buffer;
    VkSemaphore m_image_available;
    VkSemaphore m_submit_completed;
    VkFence m_in_flight_fence;
    uint32_t m_image_index;
    vulkan::DeviceQueues m_device_queues;

    RenderPass(vulkan::CommandBuffer &command_buffer, SwapChain *swap_chain);
    ~RenderPass() = default;

    RenderPass(RenderPass &&other) noexcept = default;
    RenderPass &operator=(RenderPass &&other) noexcept = default;

    RenderPass(const RenderPass &other) = delete;
    RenderPass &operator=(const RenderPass &other) = delete;

    void begin();
    void set_viewport(const Dimension &dim);
    void set_scissor(const VkExtent2D &extent);
    void end();
    void submit();
    void present();

    void end_submit_present();
};
