#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/resources/shaders/ShaderResource.h"
#include "vulkan/vulkan_core.h"
class ShaderModule {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;

    VkShaderModule create_shader_module(const uint8_t *data, const size_t len);

  public:
    VkShaderModule shader_module;

    ShaderModule(std::shared_ptr<CoreGraphicsContext> ctx, const ShaderResource &shader);
    ~ShaderModule();
};
