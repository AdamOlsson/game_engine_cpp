#include "VertexBuffer.h"
#include "render_engine/buffers/common.h"
#include <vector>

std::unique_ptr<VertexBuffer>
createVertexBuffer(std::shared_ptr<CoreGraphicsContext> &ctx,
                   const std::vector<Vertex> &vertices, const VkCommandPool &commandPool,
                   const VkQueue &graphicsQueue) {

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(
        ctx->physicalDevice, ctx->device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(ctx->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(ctx->device, stagingBufferMemory);

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    createBuffer(ctx->physicalDevice, ctx->device, bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

    copyBuffer(ctx->device, stagingBuffer, vertexBuffer, bufferSize, commandPool,
               graphicsQueue);
    vkDestroyBuffer(ctx->device, stagingBuffer, nullptr);
    vkFreeMemory(ctx->device, stagingBufferMemory, nullptr);

    return std::make_unique<VertexBuffer>(ctx, vertexBuffer, vertexBufferMemory,
                                          bufferSize);
}

VertexBuffer::~VertexBuffer() { cleanup(); }

void VertexBuffer::cleanup() {

    if (cleanup_done) {
        return;
    }

    vkDestroyBuffer(ctx->device, buffer, nullptr);
    vkFreeMemory(ctx->device, bufferMemory, nullptr);
    cleanup_done = true;
}
