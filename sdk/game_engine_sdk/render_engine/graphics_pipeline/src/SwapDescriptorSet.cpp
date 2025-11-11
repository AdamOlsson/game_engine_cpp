#include "graphics_pipeline/SwapDescriptorSet.h"
#include "vulkan/DescriptorSet.h"
#include <memory>

graphics_pipeline::SwapDescriptorSet::SwapDescriptorSet(
    std::shared_ptr<vulkan::context::GraphicsContext> ctx,
    std::vector<vulkan::DescriptorSet> &descriptor_sets,
    vulkan::DescriptorSetLayout &&layout)
    : m_ctx(ctx), m_capacity(descriptor_sets.size()), m_next(0),
      m_layout(std::move(layout)), m_descriptor_sets(std::move(descriptor_sets)) {}

const vulkan::DescriptorSet graphics_pipeline::SwapDescriptorSet::get() {
    auto &desc = m_descriptor_sets[m_next];
    m_next = ++m_next % m_capacity;
    return desc;
}
