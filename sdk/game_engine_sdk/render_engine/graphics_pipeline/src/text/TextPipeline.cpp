#include "graphics_pipeline/text/TextPipeline.h"
#include "shaders/text_fragment_shader.h"
#include "shaders/text_vertex_shader.h"
#include "vulkan/vulkan_core.h"

graphics_pipeline::text::TextPipeline::TextPipeline(
    std::shared_ptr<vulkan::context::GraphicsContext> ctx,
    vulkan::CommandBufferManager *command_buffer_manager,
    vulkan::SwapChainManager *swap_chain_manager,
    const vulkan::DescriptorSetLayout *descriptor_set_layout,
    const vulkan::PushConstantRange *push_constant_range)
    : m_ctx(ctx), m_glyph_vertex_buffer(std::nullopt), m_glyph_index_buffer(std::nullopt),
      m_push_constant_stage(push_constant_range ? push_constant_range->stageFlags : 0),
      m_pipeline_layout(
          vulkan::PipelineLayout(ctx, descriptor_set_layout, push_constant_range)) {

    auto quad_vert = TextVertexShader::create_resource();
    auto quad_frag = TextFragmentShader::create_resource();

    m_pipeline = vulkan::Pipeline(
        ctx, m_pipeline_layout,
        vulkan::ShaderModule(ctx, quad_vert->bytes(), quad_vert->length()),
        vulkan::ShaderModule(ctx, quad_frag->bytes(), quad_frag->length()),
        *swap_chain_manager,
        vulkan::PipelineOpts{
            .assembler = {.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST},
        });
}

void graphics_pipeline::text::TextPipeline::load_font(
    vulkan::CommandBufferManager *command_buffer_manager, const font::OTFFont &font) {

    std::vector<vulkan::DrawIndirectCommand> draw_commands;
    size_t instance_offset_count = 0;
    size_t offset_count = 0;
    std::vector<Vertex> vertices;
    /*size_t count = 0;*/
    for (const auto &glyph : font.glyphs) {

        /*if (count != 54 && count != 55) {*/
        /*    count++;*/
        /*    continue;*/
        /*}*/
        /*count++;*/

        // Note: When I allow of composite glyphs, the cmap will not longer be valid
        // as each additional outline of one glyph offsets all following vertices one
        // index
        const auto &outline = glyph.outlines[0];

        draw_commands.push_back(vulkan::DrawIndirectCommand{
            .vertexCount = static_cast<uint32_t>(outline.size()),
            .instanceCount = 1,
            .firstVertex = static_cast<uint32_t>(offset_count),
            .firstInstance = static_cast<uint32_t>(instance_offset_count),
        });

        /*std::cout << "vertexCount: " << draw_commands.back().vertexCount << std::endl;*/
        /*std::cout << "instanceCount: " << draw_commands.back().instanceCount <<
         * std::endl;*/
        /*std::cout << "firstVertex: " << draw_commands.back().firstVertex << std::endl;*/
        /*std::cout << "firstInstance: " << draw_commands.back().firstInstance <<
         * std::endl;*/
        /*std::cout << std::endl;*/

        // TODO: Handle multiple instances of a glyph
        instance_offset_count++;

        for (const auto &point : outline) {
            vertices.emplace_back(static_cast<float>(point.first),
                                  -1.0f * static_cast<float>(point.second), 0.0f);
            offset_count++;
        }
    }

    m_glyph_vertex_buffer =
        vulkan::buffers::VertexBuffer(m_ctx, vertices, command_buffer_manager);

    m_draw_command_buffer = vulkan::buffers::StorageBuffer<vulkan::DrawIndirectCommand>(
        m_ctx, draw_commands.size(), VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);

    /*std::cout << "Size of draw commands: " << draw_commands.size() << std::endl;*/
    for (auto &command : draw_commands) {
        m_draw_command_buffer->push_back(std::move(command));
    }

    m_draw_command_buffer->transfer();
    m_font = font;
}
