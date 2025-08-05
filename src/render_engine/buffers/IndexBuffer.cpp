#include "IndexBuffer.h"
#include "render_engine/buffers/StagingBuffer.h"
#include "render_engine/buffers/common.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <vector>

IndexBuffer::IndexBuffer()
    : m_ctx(nullptr), buffer(VK_NULL_HANDLE), bufferMemory(VK_NULL_HANDLE), size(0),
      num_indices(0) {}

IndexBuffer::IndexBuffer(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                         const std::vector<uint16_t> &indices,
                         CommandBufferManager *command_buffer_manager)
    : m_ctx(ctx), size(sizeof(uint16_t) * indices.size()),
      num_indices(size / sizeof(uint16_t)) {

    auto [graphics_queue, _] = m_ctx->get_device_queues();

    StagingBuffer staging_buffer = StagingBuffer(m_ctx, size);

    create_buffer(m_ctx.get(), size,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

    staging_buffer.transfer_data_to_device_buffer(command_buffer_manager, indices,
                                                  buffer);
}

IndexBuffer::~IndexBuffer() {
    vkDestroyBuffer(m_ctx->logical_device, buffer, nullptr);
    vkFreeMemory(m_ctx->logical_device, bufferMemory, nullptr);
}
