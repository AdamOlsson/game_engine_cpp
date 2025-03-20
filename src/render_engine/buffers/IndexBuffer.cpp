#include "IndexBuffer.h"
#include "render_engine/buffers/common.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <vector>

IndexBuffer::IndexBuffer()
    : ctx(nullptr), buffer(VK_NULL_HANDLE), bufferMemory(VK_NULL_HANDLE), size(0),
      num_indices(0) {}

IndexBuffer::IndexBuffer(std::shared_ptr<CoreGraphicsContext> ctx,
                         const std::vector<uint16_t> &indices,
                         const VkCommandPool &commandPool, const VkQueue &graphicsQueue)
    : ctx(ctx), size(sizeof(uint16_t) * indices.size()),
      num_indices(size / sizeof(uint16_t)) {

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(ctx->physicalDevice, ctx->device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(ctx->device, stagingBufferMemory, 0, size, 0, &data);
    memcpy(data, indices.data(), (size_t)size);
    vkUnmapMemory(ctx->device, stagingBufferMemory);

    createBuffer(ctx->physicalDevice, ctx->device, size,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

    copyBuffer(ctx->device, stagingBuffer, buffer, size, commandPool, graphicsQueue);

    vkDestroyBuffer(ctx->device, stagingBuffer, nullptr);
    vkFreeMemory(ctx->device, stagingBufferMemory, nullptr);
}

IndexBuffer::~IndexBuffer() {
    vkDestroyBuffer(ctx->device, buffer, nullptr);
    vkFreeMemory(ctx->device, bufferMemory, nullptr);
}
