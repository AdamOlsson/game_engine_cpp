#pragma once

#include "render_engine/graphics_context/GraphicsContext.h"
#include "render_engine/resources/shaders/ShaderResource.h"
#include "vulkan/vulkan_core.h"
class ShaderModule {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;

    VkShaderModule create_shader_module(const uint8_t *data, const size_t len);

  public:
    VkShaderModule shader_module;

    ShaderModule(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                 const ShaderResource &shader);
    ~ShaderModule();
};
