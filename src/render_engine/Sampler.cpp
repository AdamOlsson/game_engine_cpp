#include "Sampler.h"
#include "render_engine/CoreGraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>

VkSampler create_sampler(const CoreGraphicsContext *ctx);

Sampler::Sampler() : ctx(nullptr), sampler(VK_NULL_HANDLE) {}

Sampler::Sampler(std::shared_ptr<CoreGraphicsContext> ctx)
    : ctx(ctx), sampler(create_sampler(ctx.get())) {}

Sampler::~Sampler() {
    if (sampler == VK_NULL_HANDLE) {
        return;
    }
    vkDestroySampler(ctx->logical_device, sampler, nullptr);
}

Sampler::Sampler(Sampler &&other) noexcept
    : ctx(std::move(other.ctx)), sampler(other.sampler) {
    other.sampler = VK_NULL_HANDLE;
}

Sampler &Sampler::operator=(Sampler &&other) noexcept {
    if (this != &other) {
        ctx = std::move(other.ctx);
        sampler = other.sampler;

        other.sampler = VK_NULL_HANDLE;
    }

    return *this;
}

VkDescriptorSetLayoutBinding
Sampler::create_descriptor_set_layout_binding(const size_t binding_num) {
    VkDescriptorSetLayoutBinding sampler_layout_binding{};
    sampler_layout_binding.binding = binding_num;
    sampler_layout_binding.descriptorCount = 1;
    sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    sampler_layout_binding.pImmutableSamplers = nullptr;
    sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    return sampler_layout_binding;
}

VkSampler create_sampler(const CoreGraphicsContext *ctx) {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(ctx->physical_device, &properties);

    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
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
    if (vkCreateSampler(ctx->logical_device, &sampler_info, nullptr, &sampler) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create sampler");
    }
    return sampler;
}
