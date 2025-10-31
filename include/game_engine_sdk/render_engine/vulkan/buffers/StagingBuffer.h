#pragma once

#include "game_engine_sdk/render_engine/vulkan/CommandBufferManager.h"
#include "game_engine_sdk/render_engine/vulkan/TextureImage.h"
#include "game_engine_sdk/render_engine/vulkan/context/GraphicsContext.h"
#include "logger/logger.h"
#include <memory>

namespace vulkan::buffers {
struct Buffer {
    VkBuffer buffer;
    VkDeviceMemory buffer_memory;
};

class StagingBuffer {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;
    const size_t m_staging_buffer_size;
    Buffer m_staging_buffer;

    Buffer create_staging_buffer();
    void map_memory(const image::Image &image);
    void copy_buffer_to_image(CommandBufferManager *command_buffer_manager,
                              const VkImage &image, const image::ImageDimensions &dim);

    void copy_buffer_to_buffer(CommandBufferManager *command_buffer_manager,
                               const VkBuffer &dst_buffer);

  public:
    StagingBuffer() = delete;
    StagingBuffer(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                  const size_t buffer_size);
    StagingBuffer(StagingBuffer &&other) noexcept = delete;
    StagingBuffer(const StagingBuffer &other) = delete;
    StagingBuffer &operator=(StagingBuffer &&other) noexcept = delete;
    StagingBuffer &operator=(const StagingBuffer &other) = delete;
    ~StagingBuffer();

    void transfer_image_to_device_image(CommandBufferManager *command_buffer_manager,
                                        const image::Image &src,
                                        const vulkan::TextureImage &dst);

    template <typename T>
    void transfer_data_to_device_buffer(CommandBufferManager *command_buffer_manager,
                                        const std::vector<T> &src, const VkBuffer &dst) {

        const size_t src_size = sizeof(T) * src.size();
        if (m_staging_buffer_size < src_size) {
            logger::warning(
                "StagingBuffer::Warning source buffer is smaller than the staging "
                "buffer, causing only partial transfer of data");
        }

        void *data;
        vkMapMemory(m_ctx->logical_device, m_staging_buffer.buffer_memory, 0,
                    m_staging_buffer_size, 0, &data);
        memcpy(data, src.data(), src_size);
        vkUnmapMemory(m_ctx->logical_device, m_staging_buffer.buffer_memory);

        copy_buffer_to_buffer(command_buffer_manager, dst);
    }
};

} // namespace vulkan::buffers
