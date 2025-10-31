#include "game_engine_sdk/render_engine/vulkan/buffers/VertexBuffer.h"
#include "game_engine_sdk/render_engine/vulkan/buffers/StagingBuffer.h"
#include "game_engine_sdk/render_engine/vulkan/buffers/common.h"
#include <vector>

vulkan::buffers::VertexBuffer::VertexBuffer() {}

vulkan::buffers::VertexBuffer::VertexBuffer(
    std::shared_ptr<vulkan::context::GraphicsContext> ctx,
    const std::vector<Vertex> &vertices, CommandBufferManager *command_buffer_manager)
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

vulkan::buffers::VertexBuffer::VertexBuffer(VertexBuffer &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), buffer(other.buffer),
      bufferMemory(other.bufferMemory), size(other.size),
      num_vertices(other.num_vertices) {
    other.buffer = VK_NULL_HANDLE;
    other.bufferMemory = VK_NULL_HANDLE;
    other.size = 0;
    other.num_vertices = 0;
}

vulkan::buffers::VertexBuffer &
vulkan::buffers::VertexBuffer::operator=(VertexBuffer &&other) noexcept {
    if (this != &other) {
        if (m_ctx && buffer != VK_NULL_HANDLE) {
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

vulkan::buffers::VertexBuffer::~VertexBuffer() {
    if (buffer == VK_NULL_HANDLE) {
        return;
    }
    vkDestroyBuffer(m_ctx->logical_device, buffer, nullptr);
    vkFreeMemory(m_ctx->logical_device, bufferMemory, nullptr);
}
