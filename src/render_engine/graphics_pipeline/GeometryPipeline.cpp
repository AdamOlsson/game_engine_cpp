#include "render_engine/graphics_pipeline/GeometryPipeline.h"
#include "render_engine/Geometry.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/GpuBuffer.h"
#include "render_engine/descriptors/DescriptorPool.h"
#include "render_engine/descriptors/DescriptorSet.h"
#include "render_engine/descriptors/DescriptorSetBuilder.h"
#include "render_engine/graphics_pipeline/GraphicsPipelineBuilder.h"
#include "render_engine/resources/ResourceManager.h"
#include "render_engine/vulkan/Sampler.h"
#include "shape.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <cstring>
#include <memory>
#include <optional>

graphics_pipeline::GeometryPipeline::GeometryPipeline(
    std::shared_ptr<graphics_context::GraphicsContext> ctx,
    CommandBufferManager *command_buffer_manager, SwapChainManager &swap_chain_manager,
    SwapUniformBuffer<window::WindowDimension<float>> &uniform_buffers,
    std::optional<GeometryPipelineOptions> opts)
    : m_ctx(ctx), m_opts(opts.has_value() ? opts.value() : GeometryPipelineOptions{}),

      m_circle_instance_buffers(SwapStorageBuffer<GeometryInstanceBufferObject>(
          ctx, MAX_FRAMES_IN_FLIGHT, 1024)),
      m_triangle_instance_buffers(SwapStorageBuffer<GeometryInstanceBufferObject>(
          ctx, MAX_FRAMES_IN_FLIGHT, 1024)),
      m_rectangle_instance_buffers(SwapStorageBuffer<GeometryInstanceBufferObject>(
          ctx, MAX_FRAMES_IN_FLIGHT, 1024)),
      m_hexagon_instance_buffers(SwapStorageBuffer<GeometryInstanceBufferObject>(
          ctx, MAX_FRAMES_IN_FLIGHT, 1024)),

      m_descriptor_pool(DescriptorPool(m_ctx, m_descriptor_pool_capacity,
                                       m_num_storage_buffers, m_num_uniform_buffers,
                                       m_num_samplers))

{
    // Handle options
    if (m_opts.texture_ptr == nullptr && opts->sampler_ptr == nullptr) {
        m_empty_texture =
            std::make_optional(Texture::empty(m_ctx, command_buffer_manager));
        m_empty_sampler = std::make_optional(vulkan::Sampler(m_ctx));
        m_opts.texture_ptr = &m_empty_texture.value();
        m_opts.sampler_ptr = &m_empty_sampler.value();
    }

    m_circle_vertex_buffer =
        VertexBuffer(ctx, Geometry::circle_vertices, command_buffer_manager);
    m_circle_index_buffer =
        IndexBuffer(ctx, Geometry::circle_indices, command_buffer_manager);
    // m_descriptor_set_layout(DescriptorSetLayout().add(sampler->create_descriptor_set_layout_binding().build(m_ctx))
    m_circle_descriptor_set =
        DescriptorSetBuilder(MAX_FRAMES_IN_FLIGHT)
            //.set_descriptor_image_info(m_opts.texture_ptr->create_descriptor_image_info())
            .add_gpu_buffer(0, m_circle_instance_buffers.get_buffer_references())
            .add_gpu_buffer(1, uniform_buffers.get_buffer_references())
            .set_texture_and_sampler(2, m_opts.texture_ptr,
                                     m_opts.sampler_ptr) // Would be removed
            .build(m_ctx, m_descriptor_pool);

    m_triangle_vertex_buffer =
        VertexBuffer(ctx, Geometry::triangle_vertices, command_buffer_manager);
    m_triangle_index_buffer =
        IndexBuffer(ctx, Geometry::triangle_indices, command_buffer_manager);
    m_triangle_descriptor_set =
        DescriptorSetBuilder(MAX_FRAMES_IN_FLIGHT)
            .add_gpu_buffer(0, m_triangle_instance_buffers.get_buffer_references())
            .add_gpu_buffer(1, uniform_buffers.get_buffer_references())
            .set_texture_and_sampler(2, m_opts.texture_ptr, m_opts.sampler_ptr)
            .build(m_ctx, m_descriptor_pool);

    m_rectangle_vertex_buffer =
        VertexBuffer(ctx, Geometry::rectangle_vertices, command_buffer_manager);
    m_rectangle_index_buffer =
        IndexBuffer(ctx, Geometry::rectangle_indices, command_buffer_manager);
    m_rectangle_descriptor_set =
        DescriptorSetBuilder(MAX_FRAMES_IN_FLIGHT)
            .add_gpu_buffer(0, m_rectangle_instance_buffers.get_buffer_references())
            .add_gpu_buffer(1, uniform_buffers.get_buffer_references())
            .set_texture_and_sampler(2, m_opts.texture_ptr, m_opts.sampler_ptr)
            .build(m_ctx, m_descriptor_pool);

    m_hexagon_vertex_buffer =
        std::move(VertexBuffer(ctx, Geometry::hexagon_vertices, command_buffer_manager));
    m_hexagon_index_buffer =
        IndexBuffer(ctx, Geometry::hexagon_indices, command_buffer_manager);
    m_hexagon_descriptor_set =
        DescriptorSetBuilder(MAX_FRAMES_IN_FLIGHT)
            .add_gpu_buffer(0, m_hexagon_instance_buffers.get_buffer_references())
            .add_gpu_buffer(1, uniform_buffers.get_buffer_references())
            .set_texture_and_sampler(2, m_opts.texture_ptr, m_opts.sampler_ptr)
            .build(m_ctx, m_descriptor_pool);

    m_graphics_pipeline =
        // clang-format off
            graphics_pipeline::GraphicsPipelineBuilder()
                .set_vertex_shader(ResourceManager::get_instance().get_resource<ShaderResource>("GeometryVertex"))
                .set_fragment_shader(ResourceManager::get_instance().get_resource<ShaderResource>("GeometryFragment"))
                .set_descriptor_set_layout(&m_hexagon_descriptor_set.get_layout()) // All descriptors have same layout
                .set_push_constants({
                    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                    .offset = 0,
                    .size = sizeof(uint32_t)
                    })
                .build(m_ctx, swap_chain_manager);
    // clang-format on
}

graphics_pipeline::GeometryPipeline::~GeometryPipeline() {}

void graphics_pipeline::GeometryPipeline::render_circles(
    const VkCommandBuffer &command_buffer) {
    const auto &instance_buffer = m_circle_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        m_graphics_pipeline.bind_push_constants(
            command_buffer, VK_SHADER_STAGE_VERTEX_BIT, ShapeTypeEncoding::CircleShape);
        auto descriptor_set = m_circle_descriptor_set.get();
        m_graphics_pipeline.render(command_buffer, m_circle_vertex_buffer,
                                   m_circle_index_buffer, descriptor_set, num_instances);
    }
    m_circle_instance_buffers.rotate();
}

void graphics_pipeline::GeometryPipeline::render_triangles(
    const VkCommandBuffer &command_buffer) {
    const auto &instance_buffer = m_triangle_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        m_graphics_pipeline.bind_push_constants(
            command_buffer, VK_SHADER_STAGE_VERTEX_BIT, ShapeTypeEncoding::TriangleShape);
        auto descriptor_set = m_triangle_descriptor_set.get();
        m_graphics_pipeline.render(command_buffer, m_triangle_vertex_buffer,
                                   m_triangle_index_buffer, descriptor_set,
                                   num_instances);
    }
    m_triangle_instance_buffers.rotate();
}

void graphics_pipeline::GeometryPipeline::render_rectangles(
    const VkCommandBuffer &command_buffer) {

    const auto &instance_buffer = m_rectangle_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        m_graphics_pipeline.bind_push_constants(command_buffer,
                                                VK_SHADER_STAGE_VERTEX_BIT,
                                                ShapeTypeEncoding::RectangleShape);
        auto descriptor_set = m_rectangle_descriptor_set.get();
        m_graphics_pipeline.render(command_buffer, m_rectangle_vertex_buffer,
                                   m_rectangle_index_buffer, descriptor_set,
                                   num_instances);
    }
    m_rectangle_instance_buffers.rotate();
}

void graphics_pipeline::GeometryPipeline::render_hexagons(
    const VkCommandBuffer &command_buffer) {
    const auto &instance_buffer = m_hexagon_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        m_graphics_pipeline.bind_push_constants(
            command_buffer, VK_SHADER_STAGE_VERTEX_BIT, ShapeTypeEncoding::HexagonShape);
        auto descriptor_set = m_hexagon_descriptor_set.get();
        m_graphics_pipeline.render(command_buffer, m_hexagon_vertex_buffer,
                                   m_hexagon_index_buffer, descriptor_set, num_instances);
    }
    m_hexagon_instance_buffers.rotate();
}

StorageBuffer<graphics_pipeline::GeometryInstanceBufferObject> &
graphics_pipeline::GeometryPipeline::get_circle_instance_buffer() {
    return m_circle_instance_buffers.get_buffer();
}

StorageBuffer<graphics_pipeline::GeometryInstanceBufferObject> &
graphics_pipeline::GeometryPipeline::get_triangle_instance_buffer() {
    return m_triangle_instance_buffers.get_buffer();
}

StorageBuffer<graphics_pipeline::GeometryInstanceBufferObject> &
graphics_pipeline::GeometryPipeline::get_rectangle_instance_buffer() {
    return m_rectangle_instance_buffers.get_buffer();
}

StorageBuffer<graphics_pipeline::GeometryInstanceBufferObject> &
graphics_pipeline::GeometryPipeline::get_hexagon_instance_buffer() {
    return m_hexagon_instance_buffers.get_buffer();
}
