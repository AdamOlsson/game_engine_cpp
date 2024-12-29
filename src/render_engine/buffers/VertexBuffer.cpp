#include "VertexBuffer.h"
#include "render_engine/buffers/common.h"
#include <vector>

std::unique_ptr<VertexBuffer> createVertexBuffer(VkPhysicalDevice &physicalDevice,
                                                 VkDevice &device,
                                                 const std::vector<Vertex> &vertices,
                                                 VkCommandPool &commandPool,
                                                 VkQueue &graphicsQueue) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    createBuffer(physicalDevice, device, bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

    copyBuffer(device, stagingBuffer, vertexBuffer, bufferSize, commandPool,
               graphicsQueue);
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    return std::make_unique<VertexBuffer>(vertexBuffer, vertexBufferMemory);
}
