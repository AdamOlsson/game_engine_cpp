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

VertexBuffer::~VertexBuffer() {
    vkDestroyBuffer(m_ctx->logical_device, buffer, nullptr);
    vkFreeMemory(m_ctx->logical_device, bufferMemory, nullptr);
}
