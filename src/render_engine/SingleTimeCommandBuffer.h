#pragma once

#include "render_engine/graphics_context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>
class SingleTimeCommandBuffer {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;
    VkCommandPool m_command_pool;

    VkCommandBuffer allocate_buffer();

  public:
    VkCommandBuffer m_command_buffer; // TODO: Make private

    SingleTimeCommandBuffer(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                            VkCommandPool &command_pool);
    ~SingleTimeCommandBuffer();

    void begin();
    void end();
    void submit(const VkQueue &graphics_queue);
};
