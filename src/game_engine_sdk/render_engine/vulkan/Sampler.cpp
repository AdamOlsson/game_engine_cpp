#include "game_engine_sdk/render_engine/vulkan/Sampler.h"
#include "vulkan/vulkan_core.h"
#include <memory>

vulkan::Sampler::Sampler(std::shared_ptr<graphics_context::GraphicsContext> ctx)
    : m_ctx(ctx), m_sampler(create_sampler(vulkan::Filter::LINEAR,
                                           vulkan::SamplerAddressMode::REPEAT)) {}

vulkan::Sampler::Sampler(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                         vulkan::Filter filter, SamplerAddressMode address_mode)
    : m_ctx(ctx), m_sampler(create_sampler(filter, address_mode)) {}

vulkan::Sampler::~Sampler() {
    if (m_sampler == VK_NULL_HANDLE) {
        return;
    }
    vkDestroySampler(m_ctx->logical_device, m_sampler, nullptr);
}

vulkan::Sampler::Sampler(Sampler &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), m_sampler(other.m_sampler) {
    other.m_sampler = VK_NULL_HANDLE;
}

vulkan::Sampler &vulkan::Sampler::operator=(Sampler &&other) noexcept {
    if (this != &other) {
        if (m_sampler != VK_NULL_HANDLE) {
            vkDestroySampler(m_ctx->logical_device, m_sampler, nullptr);
        }

        m_ctx = std::move(other.m_ctx);
        m_sampler = other.m_sampler;

        other.m_sampler = VK_NULL_HANDLE;
    }

    return *this;
}

VkDescriptorSetLayoutBinding
vulkan::Sampler::create_descriptor_set_layout_binding(const size_t binding_num) {
    VkDescriptorSetLayoutBinding sampler_layout_binding{};
    sampler_layout_binding.binding = binding_num;
    sampler_layout_binding.descriptorCount = 1;
    sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    sampler_layout_binding.pImmutableSamplers = nullptr;
    sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    return sampler_layout_binding;
}

VkSampler vulkan::Sampler::create_sampler(vulkan::Filter filter,
                                          SamplerAddressMode address_mode) {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_ctx->physical_device, &properties);

    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = static_cast<VkFilter>(filter);
    sampler_info.minFilter = static_cast<VkFilter>(filter);
    sampler_info.addressModeU = static_cast<VkSamplerAddressMode>(address_mode);
    sampler_info.addressModeV = static_cast<VkSamplerAddressMode>(address_mode);
    sampler_info.addressModeW = static_cast<VkSamplerAddressMode>(address_mode);
    sampler_info.anisotropyEnable = VK_TRUE;
    sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f;

    VkSampler sampler;
    if (vkCreateSampler(m_ctx->logical_device, &sampler_info, nullptr, &sampler) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create sampler");
    }
    return sampler;
}
