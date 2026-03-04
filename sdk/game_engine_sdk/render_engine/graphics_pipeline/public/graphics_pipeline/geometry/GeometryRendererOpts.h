#pragma once

#include "vulkan/DescriptorPool.h"

namespace graphics_pipeline::geometry {

struct GeometryRendererOpts {
    vulkan::DescriptorPoolOpts pool_opts = vulkan::DescriptorPoolOpts{
        .max_num_descriptor_sets = 2,
        .num_storage_buffers = 1,
        .num_uniform_buffers = 0,
        .num_combined_image_samplers = 0,
    };

    struct {
        size_t size = 256;
    } instance_buffer_opts;
};

} // namespace graphics_pipeline::geometry
