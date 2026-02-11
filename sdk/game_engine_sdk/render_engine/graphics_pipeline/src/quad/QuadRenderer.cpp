#include "graphics_pipeline/quad/QuadRenderer.h"
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

graphics_pipeline::quad::QuadRenderer::QuadRenderer(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::CommandBufferManager *command_buffer_manager,
    vulkan::SwapChainManager *swap_chain_manager,
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
    m_instances =
        vulkan::buffers::StorageBuffer<graphics_pipeline::quad::QuadPipelineSBO>(
            m_ctx, opts.instance_buffer_opts.size, max_frames_in_flight);

    auto builder = SwapDescriptorSetBuilder(max_frames_in_flight);
    builder.add_storage_buffer(
        0, vulkan::DescriptorBufferInfo::from_vector(m_instances.get_reference()));
    builder.add_combined_image_sampler(
        2, {vulkan::DescriptorImageInfo(m_texture.view(), &m_sampler)});
    m_descriptor_sets = builder.build(ctx, m_descriptor_pool);

    const auto &layout = m_descriptor_sets.get_layout();
    m_quad_pipeline = QuadPipeline(m_ctx, command_buffer_manager, swap_chain_manager,
                                   &layout, push_constant_range);

    for (int i = opts.instance_buffer_opts.size - 1; i >= 0; i--) {
        m_instances.emplace_back();
        m_state.available_instance_ids.push(i);
    }
}

graphics_pipeline::quad::QuadSBOHandle
graphics_pipeline::quad::QuadRenderer::request_render_slot() {
    // TODO: This way of returning slots is does not perfectly front load all instances,
    // consider the case where we have stack 3,2,1,0 and request 2 slots. At this point we
    // have stack 3,2. Depending on which slot is returned first we night end up with
    // 3,2,0 OR 3,2,1 where the first case would requre the user to render 2 instances
    // (where the first is blank) and the latter would require the user to render only 1
    // instance. Instead I should transition to a datastructure like this:
    // https://www.youtube.com/watch?v=L4xOCvELWlU
    size_t id = m_state.available_instance_ids.top();
    QuadPipelineSBO *slot = &m_instances[id];
    m_state.available_instance_ids.pop();

    auto return_fn = [this](size_t id) {
        this->m_instances[id] = QuadPipelineSBO{};
        /*this->m_state.available_instance_ids.push(id);*/
    };

    return QuadSBOHandle(id, slot, return_fn);
}

void graphics_pipeline::quad::QuadRenderer::return_render_slot(
    graphics_pipeline::quad::QuadSBOHandle &handle) {
    handle.return_to_source();
}

void graphics_pipeline::quad::QuadRenderer::sync_render_slots() {
    m_instances.sync_all();
}
