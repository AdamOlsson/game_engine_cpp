#include "common.h"
#include "render_engine/SingleTimeCommandBuffer.h"
#include "render_engine/SwapChainManager.h"
#include "vulkan/vulkan_core.h"

#include <cstdint>
#include <stdexcept>

void create_buffer(const graphics_context::GraphicsContext *ctx, const VkDeviceSize size,
                   const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties,
                   VkBuffer &buffer, VkDeviceMemory &buffer_memory) {
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(ctx->logical_device, &buffer_info, nullptr, &buffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(ctx->logical_device, buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex =
        find_memory_type(ctx, mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(ctx->logical_device, &alloc_info, nullptr, &buffer_memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(ctx->logical_device, buffer, buffer_memory, 0);
}

uint32_t find_memory_type(const graphics_context::GraphicsContext *ctx,
                          const uint32_t type_filter,
                          const VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(ctx->physical_device, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((type_filter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");
}

void copy_buffer(const graphics_context::GraphicsContext *ctx, const VkBuffer src_buffer,
                 const VkBuffer dst_buffer, const VkDeviceSize size,
                 SwapChainManager &swap_chain_manager) {
    SingleTimeCommandBuffer command_buffer =
        swap_chain_manager.get_single_time_command_buffer();
    command_buffer.begin();

    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0; // Optional
    copy_region.dstOffset = 0; // Optional
    copy_region.size = size;
    command_buffer.copy_buffer(src_buffer, dst_buffer, copy_region);

    command_buffer.end();
    command_buffer.submit();
}
