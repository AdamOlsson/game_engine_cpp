#include "common.h"
#include "render_engine/SingleTimeCommandBuffer.h"
#include "render_engine/SwapChainManager.h"
#include "vulkan/vulkan_core.h"

#include <cstdint>
#include <stdexcept>

void createBuffer(const VkPhysicalDevice &physicalDevice, VkDevice &device,
                  const VkDeviceSize size, const VkBufferUsageFlags usage,
                  const VkMemoryPropertyFlags properties, VkBuffer &buffer,
                  VkDeviceMemory &bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex =
        findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

uint32_t findMemoryType(const VkPhysicalDevice &physicalDevice, const uint32_t typeFilter,
                        const VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");
}

void copy_buffer(const VkDevice &device, const VkBuffer src_buffer,
                 const VkBuffer dst_buffer, const VkDeviceSize size,
                 SwapChainManager &swap_chain_manager, const VkQueue &graphics_queue) {
    SingleTimeCommandBuffer command_buffer =
        swap_chain_manager.get_single_time_command_buffer();
    command_buffer.begin();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(command_buffer.m_command_buffer, src_buffer, dst_buffer, 1,
                    &copyRegion);

    command_buffer.end();
    command_buffer.submit(graphics_queue);
}

VkImageView create_image_view(const VkDevice &device, const VkImage &image,
                              const VkFormat &format) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.format = format;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    VkImageView image_view;
    if (vkCreateImageView(device, &createInfo, nullptr, &image_view) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image views!");
    }
    return image_view;
}
