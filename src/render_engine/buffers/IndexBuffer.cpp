#include "IndexBuffer.h"
#include "render_engine/buffers/common.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <vector>

IndexBuffer::IndexBuffer()
    : m_ctx(nullptr), buffer(VK_NULL_HANDLE), bufferMemory(VK_NULL_HANDLE), size(0),
      num_indices(0) {}

IndexBuffer::IndexBuffer(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                         const std::vector<uint16_t> &indices,
                         SwapChainManager &swap_chain_manager)
    : m_ctx(ctx), size(sizeof(uint16_t) * indices.size()),
      num_indices(size / sizeof(uint16_t)) {

    auto [graphics_queue, _] = m_ctx->get_device_queues();

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    create_buffer(m_ctx.get(), size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  staging_buffer, staging_buffer_memory);

    void *data;
    vkMapMemory(m_ctx->logical_device, staging_buffer_memory, 0, size, 0, &data);
    memcpy(data, indices.data(), (size_t)size);
    vkUnmapMemory(m_ctx->logical_device, staging_buffer_memory);

    create_buffer(m_ctx.get(), size,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

    copy_buffer(m_ctx.get(), staging_buffer, buffer, size, swap_chain_manager);

    vkDestroyBuffer(m_ctx->logical_device, staging_buffer, nullptr);
    vkFreeMemory(m_ctx->logical_device, staging_buffer_memory, nullptr);
}

IndexBuffer::~IndexBuffer() {
    vkDestroyBuffer(m_ctx->logical_device, buffer, nullptr);
    vkFreeMemory(m_ctx->logical_device, bufferMemory, nullptr);
}
