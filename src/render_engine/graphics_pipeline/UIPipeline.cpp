#include "UIPipeline.h"
#include "render_engine/Geometry.h"
#include "render_engine/descriptors/DescriptorSetBuilder.h"
#include "render_engine/graphics_pipeline/GeometryPipeline.h"
#include "render_engine/graphics_pipeline/GraphicsPipelineBuilder.h"
#include "render_engine/resources/ResourceManager.h"
#include "render_engine/resources/shaders/ShaderResource.h"
#include "vulkan/vulkan_core.h"

graphics_pipeline::UIPipeline::UIPipeline(
    std::shared_ptr<graphics_context::GraphicsContext> ctx,
    CommandBufferManager *command_buffer_manager, SwapChainManager &swap_chain_manager)
    : m_ctx(ctx),
      m_descriptor_pool(DescriptorPool(m_ctx, m_descriptor_pool_capacity,
                                       m_num_storage_buffers, m_num_uniform_buffers,
                                       m_num_samplers)),
      m_descriptor_set(
          DescriptorSetBuilder(graphics_pipeline::MAX_FRAMES_IN_FLIGHT)
              .add_uniform_buffer(
                  0, vulkan::DescriptorBufferInfo::from_vector(
                         swap_chain_manager.get_window_size_swap_buffer_ref()))
              .build(m_ctx, m_descriptor_pool)),
      m_graphics_pipeline(
          // clang-format off
          graphics_pipeline::GraphicsPipelineBuilder()
              .set_vertex_shader(ResourceManager::get_instance().get_resource<ShaderResource>("UiVertex"))
              .set_fragment_shader(ResourceManager::get_instance().get_resource<ShaderResource>( "UiFragment"))
              .set_push_constants({
                  .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                  .offset = 0,
                  .size = sizeof(ui::ElementProperties)})
              .set_descriptor_set_layout(&m_descriptor_set.get_layout())
              .build(m_ctx, swap_chain_manager)),
      // clang-format on 
      m_vertex_buffer(m_ctx, Geometry::rectangle_vertices, command_buffer_manager),
      m_index_buffer(IndexBuffer(ctx, Geometry::rectangle_indices, command_buffer_manager)) {}

graphics_pipeline::UIPipeline::~UIPipeline() {}

void graphics_pipeline::UIPipeline::render(const VkCommandBuffer &command_buffer,
                        const ui::ElementProperties::ContainerProperties &ui_element) {

    auto descriptor_set = m_descriptor_set.get();
    
    m_graphics_pipeline.bind_push_constants(command_buffer,VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, ui_element);
    m_graphics_pipeline.render(command_buffer, m_vertex_buffer, m_index_buffer, descriptor_set, 1);
}

