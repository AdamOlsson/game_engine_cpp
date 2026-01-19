#pragma once

#include "font/Unicode.h"
#include "vulkan/DescriptorSetLayout.h"
#include "vulkan/SwapChainManager.h"
namespace graphics_pipeline::text {

// Glyph specific data like
// - kerning information
struct GlyphSBO {
    alignas(16) glm::mat4 offset = glm::mat4(1.0f);
};

// Common data for the entire string, like
// - text color
// - font size
// - text position
struct TextString {
    size_t text_id;
    // TODO: std::vector<vulkan::DrawIndexedIndirectCommand> draw_cmds;
    alignas(16) glm::mat4 model_matrix = glm::mat4(1.0f);
};

class TextRenderer {
  private:
  public:
    TextRenderer(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                 vulkan::SwapChainManager *swap_chain_manager,
                 const VkPushConstantRange *push_constant_range);
    ~TextRenderer();

    static vulkan::DescriptorSetLayout
    get_descriptor_set_layout(std::shared_ptr<vulkan::context::GraphicsContext> &ctx);

    void allocate_descriptor_set();

    TextString create_text(const font::Unicode &codepoint);
    void update_text(TextString &text);

    void render(const TextString &text);
};
} // namespace graphics_pipeline::text
