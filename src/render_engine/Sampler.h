#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include <memory>

class Sampler {
  private:
    std::shared_ptr<CoreGraphicsContext> ctx;

  public:
    VkSampler sampler;

    Sampler();
    Sampler(std::shared_ptr<CoreGraphicsContext> ctx);
    ~Sampler();

    Sampler(Sampler &&other) noexcept;
    Sampler &operator=(Sampler &&other) noexcept;

    Sampler(const Sampler &other) = delete;
    Sampler &operator=(const Sampler &other) noexcept = delete;

    static VkDescriptorSetLayoutBinding
    create_descriptor_set_layout_binding(const size_t binding_num);
};
