#pragma once

#include "render_engine/graphics_context/GraphicsContext.h"
#include <memory>

class Sampler {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;
    VkSampler m_sampler;

    VkSampler create_sampler();

  public:
    Sampler() = default;
    Sampler(std::shared_ptr<graphics_context::GraphicsContext> ctx);
    ~Sampler();

    Sampler(Sampler &&other) noexcept;
    Sampler &operator=(Sampler &&other) noexcept;

    Sampler(const Sampler &other) = delete;
    Sampler &operator=(const Sampler &other) noexcept = delete;

    operator VkSampler() const { return m_sampler; }

    static VkDescriptorSetLayoutBinding
    create_descriptor_set_layout_binding(const size_t binding_num);
};
