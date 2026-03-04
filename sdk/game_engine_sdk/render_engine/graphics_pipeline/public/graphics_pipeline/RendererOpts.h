#pragma once

#include "graphics_pipeline/geometry/GeometryRendererOpts.h"
#include "graphics_pipeline/quad/QuadRendererOpts.h"
#include "graphics_pipeline/text/TextRendererOpts.h"
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

    quad::QuadRendererOpts quad;
    geometry::GeometryRendererOpts geometry;
    text::TextRendererOpts text;
};

}; // namespace graphics_pipeline
