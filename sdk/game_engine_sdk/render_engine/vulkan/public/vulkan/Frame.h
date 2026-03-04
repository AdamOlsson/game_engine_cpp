#pragma once

#include "CommandBuffer.h"
#include "Viewport.h"
#include "context/GraphicsContext.h"
namespace vulkan {

class SwapChain;

class Frame {
  private:
    vulkan::CommandBuffer m_command_buffer;
    VkSwapchainKHR m_swap_chain;

  public:
    VkRenderPass m_render_pass;
    VkExtent2D m_render_area_extent;
    VkFramebuffer m_frame_buffer;
    VkSemaphore m_image_available;
    VkSemaphore m_submit_completed;
    VkFence m_in_flight_fence;
    uint32_t m_image_index;
    vulkan::context::DeviceQueues m_device_queues;

    Frame(vulkan::CommandBuffer &command_buffer, VkSwapchainKHR swap_chain);
    ~Frame() = default;

    Frame(Frame &&other) noexcept = default;
    Frame &operator=(Frame &&other) noexcept = default;

    Frame(const Frame &other) = delete;
    Frame &operator=(const Frame &other) = delete;

    void begin();
    void set_viewport(const vulkan::Viewport &dim);
    void set_scissor(const VkExtent2D &extent);
    void end();
    void submit();
    void present();

    void end_submit_present();
};
} // namespace vulkan
