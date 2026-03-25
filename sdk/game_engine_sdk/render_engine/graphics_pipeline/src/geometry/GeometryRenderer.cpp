#include "graphics_pipeline/geometry/GeometryRenderer.h"
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

GeometryRenderer::GeometryRenderer(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                                   vulkan::CommandBufferManager *command_buffer_manager,
                                   RendererOpts &opts)
    : m_ctx(ctx), m_quad_vertex_buffer(vulkan::buffers::VertexBuffer(
                      m_ctx, m_quad_vertices, command_buffer_manager)),
      m_quad_index_buffer(
          vulkan::buffers::IndexBuffer(m_ctx, m_quad_indices, command_buffer_manager)),
      m_draw_commands(vulkan::buffers::IndirectBuffer<vulkan::DrawIndexedIndirectCommand>(
          m_ctx, 32, 2)) {

    m_descriptor_pool = vulkan::DescriptorPool(m_ctx, opts.geometry.pool_opts);

    const size_t max_frames_in_flight = 2;
    m_sparse_set.dense = vulkan::buffers::StagedStorageBuffer<
        graphics_pipeline::geometry::GeometryPipelineSBO>(
        m_ctx, opts.geometry.instance_buffer_opts.size, max_frames_in_flight);

    auto builder = SwapDescriptorSetBuilder(max_frames_in_flight);
    builder.add_storage_buffer(
        0, vulkan::DescriptorBufferInfo::from_vector(m_sparse_set.dense.get_reference()));
    m_descriptor_sets = builder.build(ctx, m_descriptor_pool);

    graphics_pipeline::PipelineOpts pipeline_opts{};
    pipeline_opts.swap_chain.extent = opts.swap_chain.extent;
    pipeline_opts.swap_chain.render_pass = opts.swap_chain.render_pass;
    pipeline_opts.push_constant_range = opts.push_constant_range;
    pipeline_opts.descriptor.layout = GeometryRenderer::get_descriptor_set_layout(ctx);
    m_geometry_pipeline = std::make_unique<GeometryPipeline>(m_ctx, pipeline_opts);

    m_sparse_set.next_id = 0;
    m_sparse_set.dense_count = 0;
    m_sparse_set.sparse.resize(opts.geometry.instance_buffer_opts.size, INVALID_INDEX);
    m_sparse_set.reverse.reserve(opts.geometry.instance_buffer_opts.size);
    m_sparse_set.available.reserve(opts.geometry.instance_buffer_opts.size);
    m_sparse_set.available.reserve(opts.geometry.instance_buffer_opts.size);
    m_sparse_set.dense.resize(opts.geometry.instance_buffer_opts.size);
}

vulkan::DescriptorSetLayout
graphics_pipeline::geometry::GeometryRenderer::get_descriptor_set_layout(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx) {
    graphics_pipeline::DescriptorSetLayoutBuilder builder;
    builder.add_storage_buffer_binding(0, VK_SHADER_STAGE_VERTEX_BIT);
    return builder.build(ctx);
}

GeometryPipelineSBO &GeometryRenderer::get_instance(const GeometrySBOHandle &handle) {
    return m_sparse_set.dense[m_sparse_set.sparse[handle.id]];
}

GeometrySBOHandle GeometryRenderer::request_render_slot() {
    size_t id = m_sparse_set.available.empty() ? m_sparse_set.next_id++
                                               : m_sparse_set.available.back();

    DEBUG_ASSERT(id < m_sparse_set.dense.num_elements(),
                 "Error: new render slot id is larger than GpuBuffer size.");

    if (!m_sparse_set.available.empty()) {
        m_sparse_set.available.pop_back();
    }

    m_sparse_set.sparse[id] = m_sparse_set.dense_count++;
    m_sparse_set.reverse.push_back(id);

    return GeometrySBOHandle(id);
}

void GeometryRenderer::return_render_slot(GeometrySBOHandle &handle) {
    const size_t id = handle.id;
    if (!contains(id)) {
        return;
    }

    size_t dense_index = m_sparse_set.sparse[id];
    size_t last_dense_index = m_sparse_set.dense_count - 1;
    size_t last_id = m_sparse_set.reverse[last_dense_index];

    if (dense_index != last_dense_index) {
        m_sparse_set.dense[dense_index] = std::move(m_sparse_set.dense[last_dense_index]);
        m_sparse_set.reverse[dense_index] = last_id;
        m_sparse_set.sparse[last_id] = dense_index;
    }

    m_sparse_set.dense[last_dense_index] = GeometryPipelineSBO{};
    m_sparse_set.reverse.pop_back();
    m_sparse_set.sparse[id] = INVALID_INDEX;
    m_sparse_set.available.push_back(id);
    m_sparse_set.dense_count--;
}

void GeometryRenderer::sync_render_slots() { m_sparse_set.dense.sync_all(); }

bool GeometryRenderer::contains(size_t id) const {
    return id < m_sparse_set.sparse.size() && m_sparse_set.sparse[id] != INVALID_INDEX;
}

} // namespace graphics_pipeline::geometry
