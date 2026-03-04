#pragma once

#include "vulkan/Extent2D.h"
#include "vulkan/PushConstantRange.h"
#include "vulkan/RenderPass.h"

namespace graphics_pipeline {

struct RendererOpts {
    struct {
        vulkan::Extent2D extent;
        vulkan::RenderPass *render_pass;
    } swap_chain;

    vulkan::PushConstantRange push_constant_range = vulkan::PushConstantRange{
        .stageFlags = 0,
        .offset = 0,
        .size = 0,
    };
};

}; // namespace graphics_pipeline
