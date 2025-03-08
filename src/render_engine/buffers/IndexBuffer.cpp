#include "IndexBuffer.h"
#include "render_engine/buffers/common.h"
#include <vector>

std::unique_ptr<IndexBuffer> createIndexBuffer(std::shared_ptr<CoreGraphicsContext> &ctx,
                                               const std::vector<uint16_t> &indices,
                                               const VkCommandPool &commandPool,
                                               const VkQueue &graphicsQueue) {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(
        ctx->physicalDevice, ctx->device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(ctx->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(ctx->device, stagingBufferMemory);

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    createBuffer(ctx->physicalDevice, ctx->device, bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    copyBuffer(ctx->device, stagingBuffer, indexBuffer, bufferSize, commandPool,
               graphicsQueue);

    vkDestroyBuffer(ctx->device, stagingBuffer, nullptr);
    vkFreeMemory(ctx->device, stagingBufferMemory, nullptr);
    return std::make_unique<IndexBuffer>(ctx, indexBuffer, indexBufferMemory, bufferSize);
}

IndexBuffer::~IndexBuffer() { cleanup(); }

void IndexBuffer::cleanup() {

    if (cleanup_done) {
        return;
    }

    vkDestroyBuffer(ctx->device, buffer, nullptr);
    vkFreeMemory(ctx->device, bufferMemory, nullptr);
    cleanup_done = true;
}
