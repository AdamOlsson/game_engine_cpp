#include "VertexBuffer.h"
#include "render_engine/buffers/common.h"
#include <vector>

VertexBuffer::VertexBuffer() {}

VertexBuffer::VertexBuffer(std::shared_ptr<CoreGraphicsContext> ctx,
                           const std::vector<Vertex> &vertices,
                           SwapChainManager &swap_chain_manager,
                           const VkQueue &graphicsQueue)
    : ctx(ctx), size(sizeof(Vertex) * vertices.size()),
      num_vertices(num_vertices = size / sizeof(Vertex)) {

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(ctx->physicalDevice, ctx->device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(ctx->device, stagingBufferMemory, 0, size, 0, &data);
    memcpy(data, vertices.data(), size);
    vkUnmapMemory(ctx->device, stagingBufferMemory);

    createBuffer(ctx->physicalDevice, ctx->device, size,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

    copy_buffer(ctx->device, stagingBuffer, buffer, size, swap_chain_manager,
                graphicsQueue);
    vkDestroyBuffer(ctx->device, stagingBuffer, nullptr);
    vkFreeMemory(ctx->device, stagingBufferMemory, nullptr);
}

VertexBuffer::~VertexBuffer() {
    vkDestroyBuffer(ctx->device, buffer, nullptr);
    vkFreeMemory(ctx->device, bufferMemory, nullptr);
}
