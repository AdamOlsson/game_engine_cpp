#pragma once

#include "vulkan/DescriptorPool.h"
namespace graphics_pipeline::text {

struct TextRendererOpts {

    vulkan::DescriptorPoolOpts pool_opts =
        vulkan::DescriptorPoolOpts{.max_num_descriptor_sets = 2,
                                   .num_storage_buffers = 2,
                                   .num_uniform_buffers = 0,
                                   .num_combined_image_samplers = 0};
};

} // namespace graphics_pipeline::text
