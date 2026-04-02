#pragma once
#include "graphics_pipeline/Texture.h"
#include "vulkan/DescriptorPool.h"
#include "vulkan/Sampler.h"
namespace graphics_pipeline::quad {

struct QuadRendererOpts {
    vulkan::DescriptorPoolOpts pool_opts = vulkan::DescriptorPoolOpts{
        .max_num_descriptor_sets = 2,
        .num_storage_buffers = 1,
        .num_uniform_buffers = 0,
        .num_combined_image_samplers = 1,
    };

    vulkan::SamplerOpts sampler_opts = vulkan::SamplerOpts{
        .filter = vulkan::Filter::NEAREST,
        .address_mode = vulkan::SamplerAddressMode::CLAMP_TO_BORDER,
    };

    struct {
        size_t size = 256;
    } instance_buffer_opts;

    std::vector<graphics_pipeline::Texture> textures;
};
} // namespace graphics_pipeline::quad
