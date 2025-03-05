// GOAL: Implement the following geometries:
//  - TODO: Rectangle
//  - TODO: Triangle
//  - TODO: Circle
//  - TODO: Line
//  - TODO: Arrow
//  - TODO: Hektagon

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/buffers/IndexBuffer.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/buffers/VertexBuffer.h"
#include "shape.h"
#include "vulkan/vulkan_core.h"
#include <memory>
namespace Geometry_ {

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

/// Architectually the RenderableGeometry class is encompassed by GraphicsPipeline class,
/// meaning that RenderableGeometry's functions will only be called from the
/// GraphicsPipeline. This also has the lifetime implication that RenderableGeometry lives
/// at most as long as GraphicsPipeline.
class RenderableGeometry {
  private:
    const size_t num_instance_buffers = 2;
    size_t buffer_idx = 0;
    ShapeTypeEncoding shape_type_encoding;
    std::unique_ptr<VertexBuffer> vertex_buffer;
    std::unique_ptr<IndexBuffer> index_buffer;

    std::vector<std::unique_ptr<StorageBuffer>>
    create_instance_buffers(std::shared_ptr<CoreGraphicsContext> &ctx) {
        auto instance_buffers = std::vector<std::unique_ptr<StorageBuffer>>{};
        instance_buffers.resize(num_instance_buffers);
        size_t size = 1024;
        for (size_t i = 0; i < num_instance_buffers; i++) {
            instance_buffers[i] = createStorageBuffer(ctx, size);
        }
        return instance_buffers;
    }

  public:
    // TODO: This should be private but we have a whole descriptorSet problem to fix first
    std::vector<std::unique_ptr<StorageBuffer>> instance_buffers;

    RenderableGeometry(std::shared_ptr<CoreGraphicsContext> &ctx,
                       const VkCommandPool &command_pool, const VkQueue &graphics_queue,
                       const ShapeTypeEncoding shape_type_encoding,
                       const std::vector<Vertex> &vertices,
                       const std::vector<uint16_t> &indices)
        : shape_type_encoding(shape_type_encoding),
          vertex_buffer(
              std::move(createVertexBuffer(ctx, vertices, command_pool, graphics_queue))),
          index_buffer(
              std::move(createIndexBuffer(ctx, indices, command_pool, graphics_queue))),
          instance_buffers(create_instance_buffers(ctx)) {}

    ~RenderableGeometry() {}

    void record_draw_command(const VkCommandBuffer &command_buffer,
                             const VkPipeline &graphics_pipeline,
                             const VkPipelineLayout &pipeline_layout,
                             const VkDescriptorSet &descriptor_set,
                             const size_t num_instances) {
        const VkDeviceSize vertex_buffers_offset = 0;
        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          graphics_pipeline);

        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipeline_layout, 0, 1, &descriptor_set, 0, nullptr);
        vkCmdPushConstants(command_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                           sizeof(uint32_t), &shape_type_encoding);

        vkCmdBindVertexBuffers(command_buffer, 0, 1, &vertex_buffer->buffer,
                               &vertex_buffers_offset);
        vkCmdBindIndexBuffer(command_buffer, index_buffer->buffer, 0,
                             VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(command_buffer, index_buffer->num_indices, num_instances, 0, 0,
                         0);
    }

    void update_instance_buffer(std::vector<StorageBufferObject> &&instance_data) {
        instance_buffers[buffer_idx]->updateStorageBuffer(
            std::forward<std::vector<StorageBufferObject>>(instance_data));
    }
};

class Rectangle : public RenderableGeometry {
  private:
    const std::vector<uint16_t> indices = {0, 1, 2, 0, 2, 3};
    const std::vector<Vertex> vertices = {{-0.5f, -0.5f, 0.0f},
                                          {-0.5f, 0.5f, 0.0f},
                                          {0.5f, 0.5f, 0.0f},
                                          {0.5f, -0.5f, 0.0f}};

  public:
    Rectangle(std::shared_ptr<CoreGraphicsContext> &ctx,
              const VkCommandPool &command_pool, const VkQueue &graphics_queue)
        : RenderableGeometry(ctx, command_pool, graphics_queue,
                             ShapeTypeEncoding::RectangleShape, vertices, indices) {}
};

}; // namespace Geometry_
