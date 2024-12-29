#include "UniformBuffer.h"
#include "render_engine/buffers/common.h"

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

std::unique_ptr<UniformBuffer> createUniformBuffer(VkPhysicalDevice &physicalDevice,
                                                   VkDevice &device) {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void *bufferMapped;
    createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 buffer, bufferMemory);

    vkMapMemory(device, bufferMemory, 0, bufferSize, 0, &bufferMapped);
    return std::make_unique<UniformBuffer>(buffer, bufferMemory, &bufferMapped);
}

void UniformBuffer::updateUniformBuffer(uint32_t currentImage) {}

VkDescriptorSetLayout UniformBuffer::createDescriptorSetLayout(VkDevice &device,
                                                               uint32_t binding_num) {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = binding_num;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    VkDescriptorSetLayout descriptorSetLayout;
    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    return descriptorSetLayout;
}
