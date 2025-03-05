#include "RenderableGeometry.h"

Geometry::RenderableGeometry::RenderableGeometry(
    std::shared_ptr<CoreGraphicsContext> &ctx, const VkCommandPool &command_pool,
    const VkQueue &graphics_queue, const ShapeTypeEncoding shape_type_encoding,
    const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices)
    : shape_type_encoding(shape_type_encoding),
      vertex_buffer(
          std::move(createVertexBuffer(ctx, vertices, command_pool, graphics_queue))),
      index_buffer(
          std::move(createIndexBuffer(ctx, indices, command_pool, graphics_queue))),
      instance_buffers(create_instance_buffers(ctx)) {}

Geometry::RenderableGeometry::~RenderableGeometry() {}

void Geometry::RenderableGeometry::record_draw_command(
    const VkCommandBuffer &command_buffer, const VkPipeline &graphics_pipeline,
    const VkPipelineLayout &pipeline_layout, const VkDescriptorSet &descriptor_set,
    const size_t num_instances) {
    const VkDeviceSize vertex_buffers_offset = 0;
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline_layout, 0, 1, &descriptor_set, 0, nullptr);
    vkCmdPushConstants(command_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                       sizeof(uint32_t), &shape_type_encoding);

    vkCmdBindVertexBuffers(command_buffer, 0, 1, &vertex_buffer->buffer,
                           &vertex_buffers_offset);
    vkCmdBindIndexBuffer(command_buffer, index_buffer->buffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(command_buffer, index_buffer->num_indices, num_instances, 0, 0, 0);

    buffer_idx = (buffer_idx + 1) % num_instance_buffers;
}

void Geometry::RenderableGeometry::update_instance_buffer(
    std::vector<StorageBufferObject> &&instance_data) {
    instance_buffers[buffer_idx]->updateStorageBuffer(
        std::forward<std::vector<StorageBufferObject>>(instance_data));
}

std::vector<std::unique_ptr<StorageBuffer>>
Geometry::RenderableGeometry::create_instance_buffers(
    std::shared_ptr<CoreGraphicsContext> &ctx) {
    auto instance_buffers = std::vector<std::unique_ptr<StorageBuffer>>{};
    instance_buffers.resize(num_instance_buffers);
    size_t size = 1024;
    for (size_t i = 0; i < num_instance_buffers; i++) {
        instance_buffers[i] = createStorageBuffer(ctx, size);
    }
    return instance_buffers;
}

std::vector<Vertex> Geometry::generate_circle_vertices(int num_points) {
    std::vector<Vertex> vertices;
    vertices.reserve(num_points + 1);

    vertices.push_back({0.0f, 0.0f, 0.0f});

    const float radius = 0.5f;
    const float angle_increment = -2.0f * M_PI / num_points;

    for (int i = 0; i < num_points; ++i) {
        const float angle = i * angle_increment;
        const float x = radius * std::cos(angle);
        const float y = radius * std::sin(angle);

        vertices.push_back({x, y, 0.0f});
    }

    return vertices;
}

std::vector<uint16_t> Geometry::generate_circle_indices(int num_points) {
    std::vector<uint16_t> indices;
    indices.reserve(3 * (num_points - 1));

    for (uint16_t i = 1; i < num_points - 1; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    indices.push_back(0);
    indices.push_back(num_points - 1);
    indices.push_back(1);

    return indices;
}
