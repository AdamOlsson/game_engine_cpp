#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/shapes/Vertex.h"
#include "vulkan/vulkan_core.h"
#include <memory>

class VertexBuffer {
  private:
    std::shared_ptr<CoreGraphicsContext> ctx;

    VertexBuffer &operator=(const VertexBuffer &);     // Copy assignment
    VertexBuffer &operator=(VertexBuffer &&) noexcept; // Move assignment
    VertexBuffer(const VertexBuffer &);                // Copy
    VertexBuffer(VertexBuffer &&) noexcept;            // Move

  public:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    VkDeviceSize size;
    size_t num_vertices;

    VertexBuffer();
    VertexBuffer(std::shared_ptr<CoreGraphicsContext> ctx,
                 const std::vector<Vertex> &vertices,
                 SwapChainManager &swap_chain_manager, const VkQueue &graphicsQueue);

    ~VertexBuffer();
};
