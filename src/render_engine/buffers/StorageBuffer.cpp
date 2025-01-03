#include "StorageBuffer.h"
#include "render_engine/buffers/common.h"
#include "vulkan/vulkan_core.h"
#include <iostream>
#include <ostream>

std::unique_ptr<StorageBuffer> createStorageBuffer(VkPhysicalDevice &physicalDevice,
                                                   VkDevice &device, size_t capacity) {

    VkDeviceSize bufferSize = capacity * sizeof(StorageBufferObject);

    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void *bufferMapped;
    createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 buffer, bufferMemory);

    vkMapMemory(device, bufferMemory, 0, bufferSize, 0, &bufferMapped);

    auto ptr =
        std::make_unique<StorageBuffer>(buffer, bufferMemory, bufferMapped, bufferSize);

    return ptr;
}

VkDescriptorSetLayout StorageBuffer::createDescriptorSetLayout(VkDevice &device,
                                                               uint32_t binding_num) {
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding_num;
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &layoutBinding;

    VkDescriptorSetLayout descriptorSetLayout;
    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    return descriptorSetLayout;
}

void StorageBuffer::updateStorageBuffer(const std::vector<StorageBufferObject> &ssbo) {
    memcpy(bufferMapped, &ssbo[0], sizeof(ssbo[0]) * ssbo.size());
}

void StorageBuffer::dumpData() {
    // Dump the data to verify
    auto *mappedData = reinterpret_cast<StorageBufferObject *>(bufferMapped);
    std::cout << "SSBO Data Dump:" << std::endl;
    std::cout << "Size of SSBO: " << sizeof(StorageBufferObject) << std::endl;
    std::cout << "Position: " << mappedData->position.x << ", " << mappedData->position.y
              << ", " << mappedData->position.z << std::endl;
    std::cout << "Color: " << mappedData->color.x << ", " << mappedData->color.y << ", "
              << mappedData->color.z << std::endl;
    std::cout << "Rotation: " << mappedData->rotation << std::endl;

    unsigned char *byteData = reinterpret_cast<unsigned char *>(bufferMapped);
    for (size_t i = 0; i < sizeof(StorageBufferObject); i++) {
        printf("%02x ", byteData[i]);
    }
    printf("\n");
}