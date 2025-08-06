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

IndexBuffer::IndexBuffer(IndexBuffer &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), buffer(other.buffer),
      bufferMemory(other.bufferMemory), size(other.size), num_indices(other.num_indices) {
    other.buffer = VK_NULL_HANDLE;
    other.bufferMemory = VK_NULL_HANDLE;
    other.size = 0;
    other.num_indices = 0;
}

IndexBuffer &IndexBuffer::operator=(IndexBuffer &&other) noexcept {
    if (this != &other) {
        if (buffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(m_ctx->logical_device, buffer, nullptr);
            vkFreeMemory(m_ctx->logical_device, bufferMemory, nullptr);
        }

        m_ctx = std::move(other.m_ctx);
        buffer = other.buffer;
        bufferMemory = other.bufferMemory;
        size = other.size;
        num_indices = other.num_indices;

        other.buffer = VK_NULL_HANDLE;
        other.bufferMemory = VK_NULL_HANDLE;
        other.size = 0;
        other.num_indices = 0;
    }
    return *this;
}

IndexBuffer::~IndexBuffer() {
    if (buffer == VK_NULL_HANDLE) {
        return;
    }

    vkDestroyBuffer(m_ctx->logical_device, buffer, nullptr);
    vkFreeMemory(m_ctx->logical_device, bufferMemory, nullptr);
}
