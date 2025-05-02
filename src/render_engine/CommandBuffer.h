#pragma once

#include "render_engine/Dimension.h"
#include "vulkan/vulkan_core.h"

class CommandBuffer {
  private:
  public:
    VkCommandBuffer m_command_buffer;
    VkSwapchainKHR m_swap_chain;
    VkRenderPass m_render_pass;
    VkExtent2D m_render_area_extent;
    VkFramebuffer m_frame_buffer;
    VkSemaphore m_image_available;
    VkSemaphore m_submit_completed;
    VkFence m_in_flight_fence;
    uint32_t m_image_index;

    CommandBuffer() = default;
    CommandBuffer(VkCommandBuffer &command_buffer);
    ~CommandBuffer() = default;

    CommandBuffer(const CommandBuffer &other) = delete;
    CommandBuffer(CommandBuffer &&other) noexcept = default;

    CommandBuffer &operator=(const CommandBuffer &other) = delete;
    CommandBuffer &operator=(CommandBuffer &&other) noexcept = default;

    void begin_render_pass();
    void end_render_pass();
    void set_viewport(const Dimension &dim);
    void set_scissor(const VkExtent2D &extent);
    void submit_render_pass(const VkQueue graphics_queue);
    VkResult present_render_pass(const VkQueue &present_queue);
};
