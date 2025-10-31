#include "game_engine_sdk/render_engine/buffers/common.h"
#include "vulkan/vulkan_core.h"

#include <stdexcept>

void create_buffer(const vulkan::GraphicsContext *ctx, const VkDeviceSize size,
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
    alloc_info.memoryTypeIndex = ctx->physical_device.find_memory_type(
        mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(ctx->logical_device, &alloc_info, nullptr, &buffer_memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(ctx->logical_device, buffer, buffer_memory, 0);
}
