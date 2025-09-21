#include "game_engine_sdk/render_engine/vulkan/ShaderModule.h"

vulkan::ShaderModule::ShaderModule(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                                   const ShaderResource &shader)
    : m_ctx(ctx), m_shader_module(create_shader_module(shader.bytes(), shader.length())) {
}

vulkan::ShaderModule::~ShaderModule() {
    vkDestroyShaderModule(m_ctx->logical_device, m_shader_module, nullptr);
}

VkShaderModule vulkan::ShaderModule::create_shader_module(const uint8_t *data,
                                                          const size_t len) {
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = len;
    create_info.pCode = reinterpret_cast<const uint32_t *>(data);

    VkShaderModule shader_module;
    if (vkCreateShaderModule(m_ctx->logical_device, &create_info, nullptr,
                             &shader_module) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shader_module;
}
