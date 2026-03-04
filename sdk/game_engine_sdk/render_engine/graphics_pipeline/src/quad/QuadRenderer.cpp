#include "graphics_pipeline/quad/QuadRenderer.h"
#include "graphics_pipeline/SwapDescriptorSetBuilder.h"
#include "vulkan/SwapChain.h"

namespace {
const std::vector<uint16_t> m_quad_indices = {0, 1, 2, 0, 2, 3};
const std::vector<vulkan::Vertex> m_quad_vertices = {
    vulkan::Vertex(-0.5f, -0.5f, 0.0f),
    vulkan::Vertex(-0.5f, 0.5f, 0.0f),
    vulkan::Vertex(0.5f, 0.5f, 0.0f),
    vulkan::Vertex(0.5f, -0.5f, 0.0f),
};
} // namespace

graphics_pipeline::quad::QuadRenderer::QuadRenderer(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::CommandBufferManager *command_buffer_manager, vulkan::SwapChain *swap_chain,
    const VkPushConstantRange *push_constant_range, QuadRendererOpts &&opts)
    : m_ctx(ctx), m_quad_vertex_buffer(vulkan::buffers::VertexBuffer(
                      m_ctx, m_quad_vertices, command_buffer_manager)),
      m_quad_index_buffer(
          vulkan::buffers::IndexBuffer(m_ctx, m_quad_indices, command_buffer_manager)) {

    m_descriptor_pool = vulkan::DescriptorPool(m_ctx, opts.pool_opts);
    m_sampler = vulkan::Sampler(m_ctx, opts.sampler_opts);
    m_texture = opts.texture.has_value() ? std::move(opts.texture.value())
                                         : Texture::empty(ctx, command_buffer_manager);

    const size_t max_frames_in_flight = 2;
    m_sparse_set.dense =
        vulkan::buffers::StorageBuffer<graphics_pipeline::quad::QuadPipelineSBO>(
            m_ctx, opts.instance_buffer_opts.size, max_frames_in_flight);

    auto builder = SwapDescriptorSetBuilder(max_frames_in_flight);
    builder.add_storage_buffer(
        0, vulkan::DescriptorBufferInfo::from_vector(m_sparse_set.dense.get_reference()));
    builder.add_combined_image_sampler(
        2, {vulkan::DescriptorImageInfo(m_texture.view(), &m_sampler)});
    m_descriptor_sets = builder.build(ctx, m_descriptor_pool);

    graphics_pipeline::PipelineOpts pipeline_opts{};
    pipeline_opts.swap_chain.extent = swap_chain->get_extent();
    pipeline_opts.swap_chain.render_pass = &swap_chain->m_render_pass;
    pipeline_opts.push_constant_range = *push_constant_range;
    pipeline_opts.descriptor.layout = QuadRenderer::get_descriptor_set_layout(ctx);
    m_quad_pipeline = QuadPipeline(m_ctx, pipeline_opts);

    m_sparse_set.next_id = 0;
    m_sparse_set.dense_count = 0;
    m_sparse_set.sparse.resize(opts.instance_buffer_opts.size, INVALID_INDEX);
    m_sparse_set.reverse.reserve(opts.instance_buffer_opts.size);
    m_sparse_set.available.reserve(opts.instance_buffer_opts.size);
    m_sparse_set.dense.resize(opts.instance_buffer_opts.size);
}

vulkan::DescriptorSetLayout
graphics_pipeline::quad::QuadRenderer::get_descriptor_set_layout(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx) {
    graphics_pipeline::DescriptorSetLayoutBuilder builder;
    builder.add_storage_buffer_binding(0, VK_SHADER_STAGE_VERTEX_BIT);
    builder.add_combined_image_sampler_binding(2, 1);
    return builder.build(ctx);
}

graphics_pipeline::quad::QuadPipelineSBO &
graphics_pipeline::quad::QuadRenderer::get_instance(
    const graphics_pipeline::quad::QuadSBOHandle &handle) {
    return m_sparse_set.dense[m_sparse_set.sparse[handle.id]];
}

graphics_pipeline::quad::QuadSBOHandle
graphics_pipeline::quad::QuadRenderer::request_render_slot() {
    size_t id = m_sparse_set.available.empty() ? m_sparse_set.next_id++
                                               : m_sparse_set.available.back();

    DEBUG_ASSERT(id < m_sparse_set.dense.num_elements(),
                 "Error: new render slot id is larger than GpuBuffer size.");

    if (!m_sparse_set.available.empty()) {
        m_sparse_set.available.pop_back();
    }

    m_sparse_set.sparse[id] = m_sparse_set.dense_count++;
    m_sparse_set.reverse.push_back(id);

    return QuadSBOHandle(id);
}

bool graphics_pipeline::quad::QuadRenderer::contains(size_t id) const {
    return id < m_sparse_set.sparse.size() && m_sparse_set.sparse[id] != INVALID_INDEX;
}

void graphics_pipeline::quad::QuadRenderer::return_render_slot(
    graphics_pipeline::quad::QuadSBOHandle &handle) {
    const size_t id = handle.id;
    if (!contains(id)) {
        return;
    }

    size_t dense_index = m_sparse_set.sparse[id];
    size_t last_dense_index = m_sparse_set.dense_count - 1;
    size_t last_id = m_sparse_set.reverse[last_dense_index];

    // Swap with last element if not already last
    if (dense_index != last_dense_index) {
        m_sparse_set.dense[dense_index] = std::move(m_sparse_set.dense[last_dense_index]);
        m_sparse_set.reverse[dense_index] = last_id;
        m_sparse_set.sparse[last_id] = dense_index;
    }

    // Remove last element and clean up
    m_sparse_set.dense[last_dense_index] = QuadPipelineSBO{};
    m_sparse_set.reverse.pop_back();
    m_sparse_set.sparse[id] = INVALID_INDEX;
    m_sparse_set.available.push_back(id);
    m_sparse_set.dense_count--;
}

void graphics_pipeline::quad::QuadRenderer::sync_render_slots() {
    m_sparse_set.dense.sync_all();
}
