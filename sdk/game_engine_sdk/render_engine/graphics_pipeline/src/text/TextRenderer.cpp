#include "graphics_pipeline/text/TextRenderer.h"
#include "graphics_pipeline/DescriptorSetLayoutBuilder.h"
#include "graphics_pipeline/text/TextPipeline.h"

graphics_pipeline::text::TextRenderer::TextRenderer(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::SwapChainManager *swap_chain_manager,
    const VkPushConstantRange *push_constant_range) {

    const auto layout = TextRenderer::get_descriptor_set_layout(ctx);
    TextPipeline m_text_pipeline =
        TextPipeline(ctx, swap_chain_manager, &layout, push_constant_range);

    // TODO: Create the text string storage buffer
    // TODO: Create the glyph data storage buffer
}

vulkan::DescriptorSetLayout
graphics_pipeline::text::TextRenderer::get_descriptor_set_layout(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx) {
    graphics_pipeline::DescriptorSetLayoutBuilder builder;
    builder.add_storage_buffer_binding(0,
                                       VK_SHADER_STAGE_VERTEX_BIT); // TextString buffer

    builder.add_storage_buffer_binding(1,
                                       VK_SHADER_STAGE_VERTEX_BIT); // Glyph buffer
    return builder.build(ctx);
}

void graphics_pipeline::text::TextRenderer::allocate_descriptor_set() {
    // TODO: Allocate descriptor set using above layout and return the descriptor set (we
    // hand over the ownership)
}

graphics_pipeline::text::TextString
graphics_pipeline::text::TextRenderer::create_text(const font::Unicode &codepoint) {

    TextString text = TextString{};

    for (const auto character : codepoint) {
        //  - TODO: lookup draw command and append to text.draw_cmds
        //  - TODO: lookup glyph offset and append to text.glyph_data
    }

    return text;
}

void graphics_pipeline::text::TextRenderer::update_text(TextString &text) {
    // TODO: Based on the provided text text.text_id we can update the instance buffer
}

void graphics_pipeline::text::TextRenderer::render(const TextString &text) {
    // TODO: for each draw command in text.draw_cmds
    // - make a indexed render call using the draw command
    // - TODO: How do I bind the text string instance buffer
    // - TODO: How do I bind the glyph instance buffer
}
