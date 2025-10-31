#pragma once

#include "context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"

namespace vulkan {
class ShaderModule {
  private:
    std::shared_ptr<context::GraphicsContext> m_ctx;
    VkShaderModule m_shader_module;

    VkShaderModule create_shader_module(const uint8_t *data, const size_t len);

  public:
    ShaderModule() = default;
    ShaderModule(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                 const uint8_t *data, const size_t len);
    ~ShaderModule();

    ShaderModule(ShaderModule &&other) noexcept = default;
    ShaderModule &operator=(ShaderModule &&other) noexcept = default;
    ShaderModule(const ShaderModule &other) = delete;
    ShaderModule &operator=(const ShaderModule &other) = delete;

    operator VkShaderModule() const { return m_shader_module; }
};
} // namespace vulkan
