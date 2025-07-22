#include "render_engine/ShaderModule.h"

ShaderModule::ShaderModule(std::shared_ptr<CoreGraphicsContext> ctx,
                           const ShaderResource &shader)
    : m_ctx(ctx), shader_module(create_shader_module(shader.bytes(), shader.length())) {}

ShaderModule::~ShaderModule() {
    vkDestroyShaderModule(m_ctx->device, shader_module, nullptr);
}

VkShaderModule ShaderModule::create_shader_module(const uint8_t *data, const size_t len) {
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = len;
    create_info.pCode = reinterpret_cast<const uint32_t *>(data);

    VkShaderModule shader_module;
    if (vkCreateShaderModule(m_ctx->device, &create_info, nullptr, &shader_module) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shader_module;
}
