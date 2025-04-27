#pragma once

#include "render_engine/CommandHandler.h"
#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/ImageData.h"
#include "render_engine/TextureImage.h"
#include <memory>

struct Buffer {
    VkBuffer buffer;
    VkDeviceMemory buffer_memory;
};

class StagingBuffer {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;
    const size_t m_staging_buffer_size;
    Buffer m_staging_buffer;

  public:
    Buffer create_staging_buffer();
    void map_memory(const ImageData &image);
    void copy_buffer_to_image(const VkCommandPool &command_pool,
                              const VkQueue &graphics_queue, const VkImage &image,
                              const ImageDimension &dim);

  public:
    StagingBuffer() = delete;
    StagingBuffer(std::shared_ptr<CoreGraphicsContext> ctx, const size_t buffer_size);
    StagingBuffer(StagingBuffer &&other) noexcept = delete;
    StagingBuffer(const StagingBuffer &other) = delete;
    StagingBuffer &operator=(StagingBuffer &&other) noexcept = delete;
    StagingBuffer &operator=(const StagingBuffer &other) = delete;
    ~StagingBuffer();

    void transfer_image_to_device_image(const ImageData &src, const TextureImage &dst,
                                        const CommandPool &command_pool,
                                        const VkQueue &graphics_queue);
};
