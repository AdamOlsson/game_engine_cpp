#pragma once

#include "vulkan/CommandBufferManager.h"
#include "vulkan/Vertex.h"
#include "vulkan/context/GraphicsContext.h"
#include <memory>
#include <vulkan/vulkan_core.h>

namespace vulkan::buffers {
class VertexBuffer {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

  public:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    VkDeviceSize size;
    size_t num_vertices;

    VertexBuffer();
    VertexBuffer(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                 const std::vector<Vertex> &vertices,
                 CommandBufferManager *command_buffer_manager);

    VertexBuffer(VertexBuffer &&other) noexcept;
    VertexBuffer &operator=(VertexBuffer &&other) noexcept;
    VertexBuffer(const VertexBuffer &other) = delete;
    VertexBuffer &operator=(const VertexBuffer &other) = delete;

    ~VertexBuffer();
};

} // namespace vulkan::buffers
