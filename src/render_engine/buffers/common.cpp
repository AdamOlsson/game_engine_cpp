#include "common.h"
#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/buffers/StorageBuffer.h"
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

VkCommandBuffer begin_single_time_commands(const VkDevice &device,
                                           const VkCommandPool &commandPool) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    // You may wish to create a separate command pool for these kinds of
    // short-lived buffers, because the implementation may be able to apply
    // memory allocation optimizations. You should use the
    // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT flag during command pool generation
    // in that cas
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void end_single_time_commands(const VkDevice &device, const VkCommandPool &command_pool,
                              const VkCommandBuffer command_buffer,
                              const VkQueue &graphics_queue) {
    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &command_buffer;

    vkQueueSubmit(graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphics_queue);

    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

void copyBuffer(const VkDevice &device, const VkBuffer src_buffer,
                const VkBuffer dst_buffer, const VkDeviceSize size,
                const VkCommandPool &command_pool, const VkQueue &graphics_queue) {
    VkCommandBuffer command_buffer = begin_single_time_commands(device, command_pool);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copyRegion);

    end_single_time_commands(device, command_pool, command_buffer, graphics_queue);
}

void copy_buffer_to_image(const VkDevice &device, const VkCommandPool &command_pool,
                          const VkQueue &graphics_queue, VkBuffer &buffer, VkImage &image,
                          const uint32_t width, const uint32_t height) {
    VkCommandBuffer command_buffer = begin_single_time_commands(device, command_pool);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(command_buffer, buffer, image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    end_single_time_commands(device, command_pool, command_buffer, graphics_queue);
}

void transition_image_layout(const VkDevice &device, const VkCommandPool &command_pool,
                             const VkQueue &graphics_queue, VkImage &image,
                             VkFormat format, VkImageLayout old_layout,
                             VkImageLayout new_layout) {
    VkCommandBuffer command_buffer = begin_single_time_commands(device, command_pool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0; // TODO
    barrier.dstAccessMask = 0; // TODO

    VkPipelineStageFlagBits2 source_stage;
    VkPipelineStageFlagBits2 destination_stage;
    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
        new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("Unsupported layout transition!");
    }

    vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0, nullptr,
                         0, nullptr, 1, &barrier);

    end_single_time_commands(device, command_pool, command_buffer, graphics_queue);
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

std::vector<StorageBuffer>
create_instance_buffers(std::shared_ptr<CoreGraphicsContext> &ctx,
                        const size_t capacity) {

    /*std::cout << "create_instance_buffers::start" << std::endl;*/
    size_t num_buffer_slots = 1024;
    std::vector<StorageBuffer> instance_buffers;
    for (auto i = 0; i < capacity; i++) {
        /*std::cout << "create_instance_buffers::loop" << std::endl;*/
        instance_buffers.emplace_back(ctx, num_buffer_slots);
    }
    /*std::cout << "create_instance_buffers::end" << std::endl;*/
    return instance_buffers;
}
