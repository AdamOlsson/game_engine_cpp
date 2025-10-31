#pragma once

#include "context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>

namespace vulkan {
class SingleTimeCommandBuffer {
  private:
    std::shared_ptr<context::GraphicsContext> m_ctx;
    VkCommandPool m_command_pool;
    VkCommandBuffer m_command_buffer;

    VkCommandBuffer allocate_buffer();

  public:
    SingleTimeCommandBuffer() = delete;
    SingleTimeCommandBuffer(std::shared_ptr<context::GraphicsContext> ctx,
                            VkCommandPool &command_pool);
    ~SingleTimeCommandBuffer();

    SingleTimeCommandBuffer(SingleTimeCommandBuffer &&) noexcept = delete;
    SingleTimeCommandBuffer &operator=(SingleTimeCommandBuffer &&) noexcept = delete;
    SingleTimeCommandBuffer(const SingleTimeCommandBuffer &) = delete;
    SingleTimeCommandBuffer &operator=(const SingleTimeCommandBuffer &) = delete;

    void copy_buffer(const VkBuffer &src, const VkBuffer &dst,
                     const VkBufferCopy &region);
    void copy_buffer_to_image(const VkBuffer &buffer, const VkImage &image,
                              const VkImageLayout image_layout,
                              const VkBufferImageCopy &region);
    void barrier(const VkPipelineStageFlagBits2 &source_stage,
                 const VkPipelineStageFlagBits2 &destination_stage,
                 const VkImageMemoryBarrier &barrier);
    void begin();
    void end();
    void submit();
};
} // namespace vulkan
