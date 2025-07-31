#pragma once

#include "render_engine/graphics_context/GraphicsContext.h"
#include "render_engine/resources/shaders/ShaderResource.h"
#include "vulkan/vulkan_core.h"
class ShaderModule {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;
    VkShaderModule m_shader_module;

    VkShaderModule create_shader_module(const uint8_t *data, const size_t len);

  public:
    ShaderModule(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                 const ShaderResource &shader);
    ~ShaderModule();

    operator VkShaderModule() const { return m_shader_module; }
};
