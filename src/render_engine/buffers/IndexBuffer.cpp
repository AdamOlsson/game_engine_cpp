#include "IndexBuffer.h"
#include "render_engine/buffers/common.h"
#include <vector>

std::unique_ptr<IndexBuffer> createIndexBuffer(VkPhysicalDevice &physicalDevice,
                                               VkDevice &device,
                                               const std::vector<uint16_t> &indices,
                                               VkCommandPool &commandPool,
                                               VkQueue &graphicsQueue) {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    createBuffer(physicalDevice, device, bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    copyBuffer(device, stagingBuffer, indexBuffer, bufferSize, commandPool,
               graphicsQueue);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
    return std::make_unique<IndexBuffer>(indexBuffer, indexBufferMemory);
}