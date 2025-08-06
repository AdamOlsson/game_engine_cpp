#include "VertexBuffer.h"
#include "render_engine/buffers/StagingBuffer.h"
#include "render_engine/buffers/common.h"
#include <vector>

VertexBuffer::VertexBuffer() {}

VertexBuffer::VertexBuffer(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                           const std::vector<Vertex> &vertices,
                           CommandBufferManager *command_buffer_manager)
    : m_ctx(ctx), size(sizeof(Vertex) * vertices.size()),
      num_vertices(num_vertices = size / sizeof(Vertex)) {

    auto [graphics_queue, _] = m_ctx->get_device_queues();

    StagingBuffer staging_buffer = StagingBuffer(m_ctx, size);

    create_buffer(m_ctx.get(), size,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

    staging_buffer.transfer_data_to_device_buffer(command_buffer_manager, vertices,
                                                  buffer);
}

VertexBuffer::VertexBuffer(VertexBuffer &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), buffer(other.buffer),
      bufferMemory(other.bufferMemory), size(other.size),
      num_vertices(other.num_vertices) {
    other.buffer = VK_NULL_HANDLE;
    other.bufferMemory = VK_NULL_HANDLE;
    other.size = 0;
    other.num_vertices = 0;
}

VertexBuffer &VertexBuffer::operator=(VertexBuffer &&other) noexcept {
    if (this != &other) {
        if (buffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(m_ctx->logical_device, buffer, nullptr);
            vkFreeMemory(m_ctx->logical_device, bufferMemory, nullptr);
        }

        m_ctx = std::move(other.m_ctx);
        buffer = other.buffer;
        bufferMemory = other.bufferMemory;
        size = other.size;
        num_vertices = other.num_vertices;

        other.buffer = VK_NULL_HANDLE;
        other.bufferMemory = VK_NULL_HANDLE;
        other.size = 0;
        other.num_vertices = 0;
    }
    return *this;
}

VertexBuffer::~VertexBuffer() {
    if (buffer == VK_NULL_HANDLE) {
        return;
    }
    vkDestroyBuffer(m_ctx->logical_device, buffer, nullptr);
    vkFreeMemory(m_ctx->logical_device, bufferMemory, nullptr);
}
