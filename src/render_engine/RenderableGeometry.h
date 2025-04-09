#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/DescriptorSet.h"
#include "render_engine/buffers/IndexBuffer.h"
#include "render_engine/buffers/VertexBuffer.h"
#include "shape.h"
#include "vulkan/vulkan_core.h"
#include <memory>
namespace Geometry {

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

std::vector<Vertex> generate_circle_vertices(int num_points);
std::vector<uint16_t> generate_circle_indices(int num_points);

/// Architectually the RenderableGeometry class is encompassed by GraphicsPipeline class,
/// meaning that RenderableGeometry's functions will only be called from the
/// GraphicsPipeline. This also has the lifetime implication that RenderableGeometry lives
/// at most as long as GraphicsPipeline.
class RenderableGeometry {
  private:
    size_t capacity;
    size_t buffer_idx;
    ShapeTypeEncoding shape_type_encoding;
    VertexBuffer vertex_buffer;
    IndexBuffer index_buffer;
    DescriptorSet descriptor_set;

  public:
    RenderableGeometry();
    RenderableGeometry(std::shared_ptr<CoreGraphicsContext> ctx,
                       const VkCommandPool &command_pool, const VkQueue &graphics_queue,
                       // Shape specific
                       const ShapeTypeEncoding shape_type_encoding,
                       const std::vector<Vertex> &vertices,
                       const std::vector<uint16_t> &indices,
                       DescriptorSet &&descriptor_set);

    ~RenderableGeometry();

    RenderableGeometry(RenderableGeometry &&other) noexcept = delete;
    RenderableGeometry &operator=(RenderableGeometry &&other) noexcept = delete;
    RenderableGeometry(const RenderableGeometry &other) = delete;
    RenderableGeometry &operator=(const RenderableGeometry &other) = delete;

    void record_draw_command(const VkCommandBuffer &command_buffer,
                             const VkPipeline &graphics_pipeline,
                             const VkPipelineLayout &pipeline_layout,
                             const size_t num_instances);

    void update_instance_buffer(std::vector<StorageBufferObject> &&instance_data);
};

// TODO: Should we make this constexpr?
const std::vector<uint16_t> circle_indices = generate_circle_indices(180);
const std::vector<Vertex> circle_vertices = generate_circle_vertices(180);
class Circle : public RenderableGeometry {
  public:
    Circle(std::shared_ptr<CoreGraphicsContext> &ctx, const VkCommandPool &command_pool,
           const VkQueue &graphics_queue, DescriptorSet &&descriptor_set)
        : RenderableGeometry(ctx, command_pool, graphics_queue,
                             ShapeTypeEncoding::CircleShape, circle_vertices,
                             circle_indices, std::move(descriptor_set)) {}
};

// TODO: Should we make this constexpr?
// TODO: Coordinates after an inscribed circle of radius 0.5 within the shape. Doing this
// might have some effect on RigidBody structure
const std::vector<uint16_t> triangle_indices = {0, 1, 2};
const std::vector<Vertex> triangle_vertices = {
    {{0.0f, -0.577f, 0.0f}, {-0.5f, 0.289f, 0.0f}, {0.5f, 0.289f, 0.0f}}};
class Triangle : public RenderableGeometry {
  public:
    Triangle(std::shared_ptr<CoreGraphicsContext> &ctx, const VkCommandPool &command_pool,
             const VkQueue &graphics_queue, DescriptorSet &&descriptor_set)
        : RenderableGeometry(ctx, command_pool, graphics_queue,
                             ShapeTypeEncoding::TriangleShape, triangle_vertices,
                             triangle_indices, std::move(descriptor_set)) {}
};

// TODO: Should we make this constexpr?
// Coordinates after an inscribed circle of radius 0.5 within the shape
const std::vector<uint16_t> rectangle_indices = {0, 1, 2, 0, 2, 3};
const std::vector<Vertex> rectangle_vertices = {
    {-0.5f, -0.5f, 0.0f}, {-0.5f, 0.5f, 0.0f}, {0.5f, 0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}};
class Rectangle : public RenderableGeometry {
  public:
    Rectangle(std::shared_ptr<CoreGraphicsContext> &ctx,
              const VkCommandPool &command_pool, const VkQueue &graphics_queue,
              DescriptorSet &&descriptor_set)
        : RenderableGeometry(ctx, command_pool, graphics_queue,
                             ShapeTypeEncoding::RectangleShape, rectangle_vertices,
                             rectangle_indices, std::move(descriptor_set)) {}
};

// TODO: Should we make this constexpr?
// Coordinates after an inscribed circle of radius 0.5 within the shape
const std::vector<uint16_t> hexagon_indices = {0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5};
const std::vector<Vertex> hexagon_vertices = {
    {-0.577350f, 0.0f, 0.0f}, {-0.28865, 0.5, 0.0f},   {0.28865f, 0.5, 0.0f},
    {0.577350f, 0.0f, 0.0f},  {0.28865f, -0.5f, 0.0f}, {-0.28865f, -0.5, 0.0f},
};
class Hexagon : public RenderableGeometry {
  public:
    Hexagon(std::shared_ptr<CoreGraphicsContext> &ctx, const VkCommandPool &command_pool,
            const VkQueue &graphics_queue, DescriptorSet &&descriptor_set)
        : RenderableGeometry(ctx, command_pool, graphics_queue,
                             ShapeTypeEncoding::HexagonShape, hexagon_vertices,
                             hexagon_indices, std::move(descriptor_set)) {}
};

}; // namespace Geometry
