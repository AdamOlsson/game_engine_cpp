#pragma once

#include "Filter.h"
#include "game_engine_sdk/render_engine/vulkan/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>

namespace vulkan {

enum class SamplerAddressMode {
    REPEAT = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    MIRRORED_REPEAT = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
    CLAMP_TO_EDGE = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    CLAMP_TO_BORDER = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
    MIRROR_CLAMP_TO_EDGE = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
};

enum class SamplerMipmapMode {
    NEAREST = VK_SAMPLER_MIPMAP_MODE_NEAREST,
    LINEAR = VK_SAMPLER_MIPMAP_MODE_LINEAR,
};

class Sampler {
  private:
    std::shared_ptr<vulkan::GraphicsContext> m_ctx;
    VkSampler m_sampler;

    VkSampler create_sampler(vulkan::Filter filter,
                             vulkan::SamplerAddressMode address_mode);

  public:
    Sampler() = default;
    Sampler(std::shared_ptr<vulkan::GraphicsContext> ctx);
    Sampler(std::shared_ptr<vulkan::GraphicsContext> ctx, vulkan::Filter filter,
            vulkan::SamplerAddressMode address_mode);
    ~Sampler();

    Sampler(Sampler &&other) noexcept;
    Sampler &operator=(Sampler &&other) noexcept;

    Sampler(const Sampler &other) = delete;
    Sampler &operator=(const Sampler &other) noexcept = delete;

    operator VkSampler() const { return m_sampler; }

    static VkDescriptorSetLayoutBinding
    create_descriptor_set_layout_binding(const size_t binding_num);
};
} // namespace vulkan
