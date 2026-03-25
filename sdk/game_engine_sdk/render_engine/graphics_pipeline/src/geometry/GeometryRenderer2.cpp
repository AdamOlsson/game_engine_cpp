
#include "graphics_pipeline/geometry/GeometryRenderer2.h"
#include "graphics_pipeline/SwapDescriptorSetBuilder.h"

namespace {

const std::vector<uint16_t> m_quad_indices = {0, 1, 2, 0, 2, 3};
const std::vector<vulkan::Vertex> m_quad_vertices = {
    vulkan::Vertex(-0.5f, -0.5f, 0.0f),
    vulkan::Vertex(-0.5f, 0.5f, 0.0f),
    vulkan::Vertex(0.5f, 0.5f, 0.0f),
    vulkan::Vertex(0.5f, -0.5f, 0.0f),
};

} // namespace

namespace graphics_pipeline::geometry {

GeometryRenderer2::GeometryRenderer2(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::CommandBufferManager *command_buffer_manager, RendererOpts &opts)
    : m_ctx(ctx), m_quad_vertex_buffer(vulkan::buffers::VertexBuffer(
                      m_ctx, m_quad_vertices, command_buffer_manager)),
      m_quad_index_buffer(
          vulkan::buffers::IndexBuffer(m_ctx, m_quad_indices, command_buffer_manager)),
      m_draw_commands(vulkan::buffers::IndirectBuffer<vulkan::DrawIndexedIndirectCommand>(
          m_ctx, 32, 2)),
      m_instances(vulkan::buffers::StorageBuffer<
                  graphics_pipeline::geometry::GeometryPipelineSBO>(
          m_ctx, opts.geometry.instance_buffer_opts.size, 2))

{

    m_descriptor_pool = vulkan::DescriptorPool(m_ctx, opts.geometry.pool_opts);

    const size_t max_frames_in_flight = 2;

    auto builder = SwapDescriptorSetBuilder(max_frames_in_flight);
    builder.add_storage_buffer(
        0, vulkan::DescriptorBufferInfo::from_vector(m_instances.get_reference()));
    m_descriptor_sets = builder.build(ctx, m_descriptor_pool);

    graphics_pipeline::PipelineOpts pipeline_opts{};
    pipeline_opts.swap_chain.extent = opts.swap_chain.extent;
    pipeline_opts.swap_chain.render_pass = opts.swap_chain.render_pass;
    pipeline_opts.push_constant_range = opts.push_constant_range;
    pipeline_opts.descriptor.layout = GeometryRenderer2::get_descriptor_set_layout(ctx);
    m_geometry_pipeline = std::make_unique<GeometryPipeline>(m_ctx, pipeline_opts);
}

vulkan::DescriptorSetLayout
graphics_pipeline::geometry::GeometryRenderer2::get_descriptor_set_layout(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx) {
    graphics_pipeline::DescriptorSetLayoutBuilder builder;
    builder.add_storage_buffer_binding(0, VK_SHADER_STAGE_VERTEX_BIT);
    return builder.build(ctx);
}

vulkan::DrawIndexedIndirectCommand GeometryRenderer2::write_to_buffer_indexed(
    const std::vector<GeometryPipelineSBO> &instance_data,
    const std::vector<size_t> &indices, const size_t offset) {

    vulkan::DrawIndexedIndirectCommand draw_command;
    draw_command.firstIndex = 0;
    draw_command.firstInstance = 0;
    draw_command.indexCount = m_quad_index_buffer.num_indices;
    draw_command.instanceCount = indices.size();
    draw_command.vertexOffset = 0;

    m_instances.write_indices(instance_data, indices, offset);
    return draw_command;
}

vulkan::DrawIndexedIndirectCommand
GeometryRenderer2::write_to_buffer(const std::vector<GeometryPipelineSBO> &instance_data,
                                   const size_t offset) {

    vulkan::DrawIndexedIndirectCommand draw_command;
    draw_command.firstIndex = 0;
    draw_command.firstInstance = 0;
    draw_command.indexCount = m_quad_index_buffer.num_indices;
    draw_command.instanceCount = instance_data.size();
    draw_command.vertexOffset = 0;

    m_instances.write(instance_data, offset);
    return draw_command;
}

} // namespace graphics_pipeline::geometry
