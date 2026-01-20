#pragma once

#include "vulkan/CommandBufferManager.h"
#include "vulkan/Vertex.h"
#include "vulkan/buffers/StagingBuffer.h"
#include "vulkan/buffers/common.h"
#include "vulkan/context/GraphicsContext.h"
#include <memory>
#include <vulkan/vulkan_core.h>

namespace vulkan::buffers {

template <typename T = vulkan::Vertex> class VertexBuffer {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

  public:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    VkDeviceSize size;
    size_t num_vertices;

    VertexBuffer() = default;
    VertexBuffer(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                 const std::vector<T> &vertices,
                 CommandBufferManager *command_buffer_manager)

        : m_ctx(ctx), size(sizeof(T) * vertices.size()), num_vertices(vertices.size()) {

        auto [graphics_queue, _] = m_ctx->get_device_queues();

        StagingBuffer staging_buffer = StagingBuffer(m_ctx, size);

        create_buffer(m_ctx.get(), size,
                      VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

        staging_buffer.transfer_data_to_device_buffer(command_buffer_manager, vertices,
                                                      buffer);
    }

    VertexBuffer(VertexBuffer &&other) noexcept
        : m_ctx(std::move(other.m_ctx)), buffer(other.buffer),
          bufferMemory(other.bufferMemory), size(other.size),
          num_vertices(other.num_vertices) {
        other.buffer = VK_NULL_HANDLE;
        other.bufferMemory = VK_NULL_HANDLE;
        other.size = 0;
        other.num_vertices = 0;
    }

    VertexBuffer &operator=(VertexBuffer &&other) noexcept {
        if (this != &other) {
            if (m_ctx && buffer != VK_NULL_HANDLE) {
                vkDestroyBuffer(m_ctx->logical_device, buffer, nullptr);
                vkFreeMemory(m_ctx->logical_device, bufferMemory, nullptr);
            }

            m_ctx = std::move(other.m_ctx);
            buffer = other.buffer;
            bufferMemory = other.bufferMemory;
            size = other.size;
            num_vertices = other.num_vertices;

            other.buffer = VK_NULL_HANDLE;
            other.bufferMemory = VK_NULL_HANDLE;
            other.size = 0;
            other.num_vertices = 0;
        }
        return *this;
    }

    VertexBuffer(const VertexBuffer &other) = delete;

    VertexBuffer &operator=(const VertexBuffer &other) = delete;

    ~VertexBuffer() {
        if (buffer == VK_NULL_HANDLE) {
            return;
        }
        vkDestroyBuffer(m_ctx->logical_device, buffer, nullptr);
        vkFreeMemory(m_ctx->logical_device, bufferMemory, nullptr);
    }
};

} // namespace vulkan::buffers
