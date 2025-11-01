#include "game_engine_sdk/render_engine/graphics_pipeline/quad/QuadPipeline.h"
#include "game_engine_sdk/render_engine/Geometry.h"
#include "game_engine_sdk/render_engine/resources/shaders/fragment/quad/quad.h"
#include "game_engine_sdk/render_engine/resources/shaders/vertex/quad/quad.h"

using namespace graphics_pipeline;

QuadPipeline::QuadPipeline(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                           vulkan::CommandBufferManager *command_buffer_manager,
                           vulkan::SwapChainManager *swap_chain_manager,
                           const vulkan::DescriptorSetLayout *descriptor_set_layout,
                           const vulkan::PushConstantRange *push_constant_range)
    : m_ctx(ctx), m_quad_vertex_buffer(vulkan::buffers::VertexBuffer(
                      m_ctx, Geometry::quad_vertices, command_buffer_manager)),
      m_quad_index_buffer(vulkan::buffers::IndexBuffer(m_ctx, Geometry::quad_indices,
                                                       command_buffer_manager)),
      m_push_constant_stage(push_constant_range ? push_constant_range->stageFlags : 0),
      m_pipeline_layout(
          vulkan::PipelineLayout(ctx, descriptor_set_layout, push_constant_range)) {

    auto quad_vert = QuadVertex::create_resource();
    auto quad_frag = QuadFragment::create_resource();

    m_pipeline = vulkan::Pipeline(
        ctx, m_pipeline_layout,
        vulkan::ShaderModule(ctx, quad_vert->bytes(), quad_vert->length()),
        vulkan::ShaderModule(ctx, quad_frag->bytes(), quad_frag->length()),
        *swap_chain_manager);
}

/*static QuadPipeline create(std::shared_ptr<vulkan::GraphicsContext> ctx,*/
/*                           CommandBufferManager *command_buffer_manager,*/
/*                           SwapChainManager *swap_chain_manager) {}*/
