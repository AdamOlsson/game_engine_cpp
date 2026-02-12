#include "graphics_pipeline/geometry/GeometryRenderer.h"
#include "graphics_pipeline/SwapDescriptorSetBuilder.h"
#include <algorithm>

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
                                   vulkan::SwapChainManager *swap_chain_manager,
                                   const vulkan::PushConstantRange *push_constant_range,
                                   GeometryRendererOpts &&opts)
    : m_ctx(ctx), m_quad_vertex_buffer(vulkan::buffers::VertexBuffer(
                      m_ctx, m_quad_vertices, command_buffer_manager)),
      m_quad_index_buffer(
          vulkan::buffers::IndexBuffer(m_ctx, m_quad_indices, command_buffer_manager)) {

    m_descriptor_pool = vulkan::DescriptorPool(m_ctx, opts.pool_opts);

    const size_t max_frames_in_flight = 2;
    m_instances =
        vulkan::buffers::StorageBuffer<graphics_pipeline::geometry::GeometryPipelineSBO>(
            m_ctx, opts.instance_buffer_opts.size, max_frames_in_flight);

    auto builder = SwapDescriptorSetBuilder(max_frames_in_flight);
    builder.add_storage_buffer(
        0, vulkan::DescriptorBufferInfo::from_vector(m_instances.get_reference()));
    m_descriptor_sets = builder.build(ctx, m_descriptor_pool);

    const auto &layout = m_descriptor_sets.get_layout();

    m_geometry_pipeline = std::make_unique<GeometryPipeline>(
        m_ctx, command_buffer_manager, swap_chain_manager, &layout, push_constant_range);

    m_state.is_dirty.resize(opts.instance_buffer_opts.size);
    std::fill(m_state.is_dirty.begin(), m_state.is_dirty.end(), false);

    for (int i = opts.instance_buffer_opts.size - 1; i >= 0; i--) {
        m_instances.emplace_back();
        m_state.available_instance_ids.push(i);
    }
}

GeometryPipelineSBO &GeometryRenderer::get_instance(const GeometrySBOHandle &handle) {
    m_state.is_dirty[handle.id] = true;
    return m_instances[handle.id];
}

GeometrySBOHandle GeometryRenderer::request_render_slot() {
    size_t id = m_next_instance_id++;

    // NOTE: No bounds checking - may crash as requested
    // TODO: Add proper capacity management later

    if (id >= m_instances.size()) {
        m_instances.emplace_back(GeometryPipelineSBO{});
        m_state.is_dirty.push_back(true);
    } else {
        m_state.is_dirty[id] = true;
    }

    return GeometrySBOHandle(id);
}

void GeometryRenderer::return_render_slot(GeometrySBOHandle &handle) {
    m_instances[handle.id] = GeometryPipelineSBO{};
    m_state.is_dirty[handle.id] = true;
}

void GeometryRenderer::sync_render_slots() {
    m_instances.sync_all();
    std::fill(m_state.is_dirty.begin(), m_state.is_dirty.end(), false);
}

} // namespace graphics_pipeline::geometry
