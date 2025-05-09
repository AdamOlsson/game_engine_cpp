#include "StagingBuffer.h"
#include "render_engine/ImageData.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/TextureImage.h"
#include "render_engine/buffers/common.h"

StagingBuffer::StagingBuffer(std::shared_ptr<CoreGraphicsContext> ctx,
                             const size_t buffer_size)
    : m_ctx(ctx), m_staging_buffer_size(buffer_size),
      m_staging_buffer(create_staging_buffer()) {}

StagingBuffer::~StagingBuffer() {
    vkDestroyBuffer(m_ctx->device, m_staging_buffer.buffer, nullptr);
    vkFreeMemory(m_ctx->device, m_staging_buffer.buffer_memory, nullptr);
}

void StagingBuffer::map_memory(const ImageData &image) {
    void *data;
    vkMapMemory(m_ctx->device, m_staging_buffer.buffer_memory, 0, image.size, 0, &data);
    memcpy(data, image.pixels, static_cast<size_t>(image.size));
    vkUnmapMemory(m_ctx->device, m_staging_buffer.buffer_memory);
}

void StagingBuffer::transfer_image_to_device_image(const ImageData &src,
                                                   const TextureImage &dst,
                                                   SwapChainManager &swap_chain_manager,
                                                   const VkQueue &graphics_queue) {

    map_memory(src);
    copy_buffer_to_image(swap_chain_manager, graphics_queue, dst.m_image, src.dimension);
}

Buffer StagingBuffer::create_staging_buffer() {
    Buffer buf{};
    create_buffer(m_ctx.get(), m_staging_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  buf.buffer, buf.buffer_memory);
    return buf;
}

void StagingBuffer::copy_buffer_to_image(SwapChainManager &swap_chain_manager,
                                         const VkQueue &graphics_queue,
                                         const VkImage &image,
                                         const ImageDimension &dim) {
    SingleTimeCommandBuffer command_buffer =
        swap_chain_manager.get_single_time_command_buffer();
    command_buffer.begin();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {dim.width, dim.height, 1};

    vkCmdCopyBufferToImage(command_buffer.m_command_buffer, m_staging_buffer.buffer,
                           image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    command_buffer.end();
    command_buffer.submit(graphics_queue);
}
