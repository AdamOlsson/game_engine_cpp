#include "DescriptorSet.h"
#include "vulkan/vulkan_core.h"
#include <memory>

DescriptorSet::DescriptorSet(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                             std::vector<VkDescriptorSet> &descriptor_sets)
    : m_ctx(ctx), m_capacity(descriptor_sets.size()), m_next(0),
      m_descriptor_sets(std::move(descriptor_sets)) {}

const VkDescriptorSet DescriptorSet::get() {
    auto &desc = m_descriptor_sets[m_next];
    m_next = ++m_next % m_capacity;
    return desc;
}
