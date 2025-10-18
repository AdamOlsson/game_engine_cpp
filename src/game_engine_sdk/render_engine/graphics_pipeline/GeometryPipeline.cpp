#include "game_engine_sdk/render_engine/graphics_pipeline/GeometryPipeline.h"
#include "game_engine_sdk/render_engine/Geometry.h"
#include "game_engine_sdk/render_engine/buffers/GpuBuffer.h"
#include "game_engine_sdk/render_engine/descriptors/SwapDescriptorSetBuilder.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/GraphicsPipelineBuilder.h"
#include "game_engine_sdk/render_engine/resources/ResourceManager.h"
#include "game_engine_sdk/render_engine/resources/shaders/fragment/geometry/geometry.h"
#include "game_engine_sdk/render_engine/resources/shaders/vertex/geometry/geometry.h"
#include "vulkan/vulkan_core.h"
#include <cstring>
#include <memory>
#include <optional>

graphics_pipeline::GeometryPipeline::GeometryPipeline(
    std::shared_ptr<graphics_context::GraphicsContext> ctx,
    CommandBufferManager *command_buffer_manager, SwapChainManager &swap_chain_manager,
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

      m_descriptor_pool(
          vulkan::DescriptorPool(m_ctx, m_descriptor_pool_capacity, m_num_storage_buffers,
                                 m_num_uniform_buffers, m_num_combined_image_samplers)),

      m_quad_vertex_buffer(
          VertexBuffer(m_ctx, Geometry::quad_vertices, command_buffer_manager)),
      m_quad_index_buffer(
          IndexBuffer(m_ctx, Geometry::quad_indices, command_buffer_manager))

{
    // Handle options
    if (m_opts.combined_image_samplers.size() == 0) {
        m_empty_texture =
            std::make_optional(Texture::empty(m_ctx, command_buffer_manager));
        m_empty_sampler = std::make_optional(vulkan::Sampler(m_ctx));
        m_opts.combined_image_samplers = {vulkan::DescriptorImageInfo(
            m_empty_texture.value().view(), &m_empty_sampler.value())};
    }

    m_circle_vertices_ubo =
        SwapUniformBuffer<VertexUBO>(m_ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT, 1);
    m_circle_vertices_ubo.write(Geometry::circle_vertices_ubo);
    m_circle_descriptor_set =
        SwapDescriptorSetBuilder(MAX_FRAMES_IN_FLIGHT)
            .add_storage_buffer(0, vulkan::DescriptorBufferInfo::from_vector(
                                       m_circle_instance_buffers.get_buffer_references()))
            .add_uniform_buffer(1,
                                vulkan::DescriptorBufferInfo::from_vector(
                                    swap_chain_manager.get_window_size_swap_buffer_ref()),
                                {.stage_flags = VK_SHADER_STAGE_VERTEX_BIT |
                                                VK_SHADER_STAGE_FRAGMENT_BIT})
            .add_combined_image_sampler(2, m_opts.combined_image_samplers)
            .add_uniform_buffer(3,
                                vulkan::DescriptorBufferInfo::from_vector(
                                    m_circle_vertices_ubo.get_buffer_references()),
                                {.stage_flags = VK_SHADER_STAGE_FRAGMENT_BIT})

            .build(m_ctx, m_descriptor_pool);

    m_triangle_vertices_ubo =
        SwapUniformBuffer<VertexUBO>(m_ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT, 1);
    m_triangle_vertices_ubo.write(Geometry::triangle_vertices_ubo);
    m_triangle_descriptor_set =
        SwapDescriptorSetBuilder(MAX_FRAMES_IN_FLIGHT)
            .add_storage_buffer(0,
                                vulkan::DescriptorBufferInfo::from_vector(
                                    m_triangle_instance_buffers.get_buffer_references()))
            .add_uniform_buffer(1,
                                vulkan::DescriptorBufferInfo::from_vector(
                                    swap_chain_manager.get_window_size_swap_buffer_ref()),
                                {.stage_flags = VK_SHADER_STAGE_VERTEX_BIT |
                                                VK_SHADER_STAGE_FRAGMENT_BIT})
            .add_combined_image_sampler(2, m_opts.combined_image_samplers)
            .add_uniform_buffer(3,
                                vulkan::DescriptorBufferInfo::from_vector(
                                    m_triangle_vertices_ubo.get_buffer_references()),
                                {.stage_flags = VK_SHADER_STAGE_FRAGMENT_BIT})
            .build(m_ctx, m_descriptor_pool);

    m_rectangle_vertices_ubo =
        SwapUniformBuffer<VertexUBO>(m_ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT, 1);
    m_rectangle_vertices_ubo.write(Geometry::rectangle_vertices_ubo);
    m_rectangle_descriptor_set =
        SwapDescriptorSetBuilder(MAX_FRAMES_IN_FLIGHT)
            .add_storage_buffer(0,
                                vulkan::DescriptorBufferInfo::from_vector(
                                    m_rectangle_instance_buffers.get_buffer_references()))
            .add_uniform_buffer(1,
                                vulkan::DescriptorBufferInfo::from_vector(
                                    swap_chain_manager.get_window_size_swap_buffer_ref()),
                                {.stage_flags = VK_SHADER_STAGE_VERTEX_BIT |
                                                VK_SHADER_STAGE_FRAGMENT_BIT})
            .add_combined_image_sampler(2, m_opts.combined_image_samplers)
            .add_uniform_buffer(3,
                                vulkan::DescriptorBufferInfo::from_vector(
                                    m_rectangle_vertices_ubo.get_buffer_references()),
                                {.stage_flags = VK_SHADER_STAGE_FRAGMENT_BIT})
            .build(m_ctx, m_descriptor_pool);

    m_hexagon_vertices_ubo =
        SwapUniformBuffer<VertexUBO>(m_ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT, 1);
    m_hexagon_vertices_ubo.write(Geometry::hexagon_vertices_ubo);
    m_hexagon_descriptor_set =
        SwapDescriptorSetBuilder(MAX_FRAMES_IN_FLIGHT)
            .add_storage_buffer(0,
                                vulkan::DescriptorBufferInfo::from_vector(
                                    m_hexagon_instance_buffers.get_buffer_references()))
            .add_uniform_buffer(1,
                                vulkan::DescriptorBufferInfo::from_vector(
                                    swap_chain_manager.get_window_size_swap_buffer_ref()),
                                {.stage_flags = VK_SHADER_STAGE_VERTEX_BIT |
                                                VK_SHADER_STAGE_FRAGMENT_BIT})
            .add_combined_image_sampler(2, m_opts.combined_image_samplers)
            .add_uniform_buffer(3,
                                vulkan::DescriptorBufferInfo::from_vector(
                                    m_hexagon_vertices_ubo.get_buffer_references()),
                                {.stage_flags = VK_SHADER_STAGE_FRAGMENT_BIT})
            .build(m_ctx, m_descriptor_pool);

    register_shader(GeometryFragment::create_resource());
    register_shader(GeometryVertex::create_resource());

    m_graphics_pipeline =
        graphics_pipeline::GraphicsPipelineBuilder()
            // clang-format off
                .set_vertex_shader(ResourceManager::get_instance().get_resource<ShaderResource>("GeometryVertex"))
                .set_fragment_shader(ResourceManager::get_instance().get_resource<ShaderResource>("GeometryFragment"))
                .set_push_constants(VkPushConstantRange{
                        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                        .offset = 0,
                        .size = sizeof(glm::mat4) })
                .set_descriptor_set_layout(&m_rectangle_descriptor_set.get_layout()) // All descriptors have same layout
                .build(m_ctx, swap_chain_manager);
    // clang-format on
}

graphics_pipeline::GeometryPipeline::~GeometryPipeline() {}

void graphics_pipeline::GeometryPipeline::render_circles(
    const VkCommandBuffer &command_buffer, glm::mat4 &camera_matrix) {
    const auto &instance_buffer = m_circle_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        m_graphics_pipeline.bind_push_constants(
            command_buffer, VK_SHADER_STAGE_VERTEX_BIT, camera_matrix);
        auto descriptor_set = m_circle_descriptor_set.get();
        m_graphics_pipeline.render(command_buffer, m_quad_vertex_buffer,
                                   m_quad_index_buffer, descriptor_set, num_instances);
    }
    m_circle_instance_buffers.rotate();
}

void graphics_pipeline::GeometryPipeline::render_triangles(
    const VkCommandBuffer &command_buffer, glm::mat4 &camera_matrix) {
    const auto &instance_buffer = m_triangle_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        m_graphics_pipeline.bind_push_constants(
            command_buffer, VK_SHADER_STAGE_VERTEX_BIT, camera_matrix);
        auto descriptor_set = m_triangle_descriptor_set.get();
        m_graphics_pipeline.render(command_buffer, m_quad_vertex_buffer,
                                   m_quad_index_buffer, descriptor_set, num_instances);
    }
    m_triangle_instance_buffers.rotate();
}

void graphics_pipeline::GeometryPipeline::render_rectangles(
    const VkCommandBuffer &command_buffer, glm::mat4 &camera_matrix) {

    const auto &instance_buffer = m_rectangle_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        m_graphics_pipeline.bind_push_constants(
            command_buffer, VK_SHADER_STAGE_VERTEX_BIT, camera_matrix);

        auto descriptor_set = m_rectangle_descriptor_set.get();
        m_graphics_pipeline.render(command_buffer, m_quad_vertex_buffer,
                                   m_quad_index_buffer, descriptor_set, num_instances);
    }
    m_rectangle_instance_buffers.rotate();
    m_rectangle_vertices_ubo.rotate();
}

void graphics_pipeline::GeometryPipeline::render_hexagons(
    const VkCommandBuffer &command_buffer, glm::mat4 &camera_matrix) {
    const auto &instance_buffer = m_hexagon_instance_buffers.get_buffer();
    const auto num_instances = instance_buffer.num_elements();
    if (num_instances > 0) {
        m_graphics_pipeline.bind_push_constants(
            command_buffer, VK_SHADER_STAGE_VERTEX_BIT, camera_matrix);
        auto descriptor_set = m_hexagon_descriptor_set.get();
        m_graphics_pipeline.render(command_buffer, m_quad_vertex_buffer,
                                   m_quad_index_buffer, descriptor_set, num_instances);
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
