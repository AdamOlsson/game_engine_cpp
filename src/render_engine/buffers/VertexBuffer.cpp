#include "VertexBuffer.h"
#include "render_engine/buffers/common.h"
#include <vector>

VertexBuffer::VertexBuffer() {}

VertexBuffer::VertexBuffer(std::shared_ptr<CoreGraphicsContext> ctx,
                           const std::vector<Vertex> &vertices,
                           SwapChainManager &swap_chain_manager)
    : m_ctx(ctx), size(sizeof(Vertex) * vertices.size()),
      num_vertices(num_vertices = size / sizeof(Vertex)) {

    auto [graphics_queue, _] = m_ctx->get_device_queues();

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    create_buffer(m_ctx.get(), size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  staging_buffer, staging_buffer_memory);

    void *data;
    vkMapMemory(m_ctx->logical_device, staging_buffer_memory, 0, size, 0, &data);
    memcpy(data, vertices.data(), size);
    vkUnmapMemory(m_ctx->logical_device, staging_buffer_memory);

    create_buffer(m_ctx.get(), size,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

    copy_buffer(m_ctx.get(), staging_buffer, buffer, size, swap_chain_manager,
                graphics_queue);
    vkDestroyBuffer(m_ctx->logical_device, staging_buffer, nullptr);
    vkFreeMemory(m_ctx->logical_device, staging_buffer_memory, nullptr);
}

VertexBuffer::~VertexBuffer() {
    vkDestroyBuffer(m_ctx->logical_device, buffer, nullptr);
    vkFreeMemory(m_ctx->logical_device, bufferMemory, nullptr);
}
