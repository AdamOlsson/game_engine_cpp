#include "game_engine_sdk/render_engine/descriptors/SwapDescriptorSet.h"
#include "game_engine_sdk/render_engine/vulkan/DescriptorSet.h"
#include <memory>

SwapDescriptorSet::SwapDescriptorSet(std::shared_ptr<vulkan::GraphicsContext> ctx,
                                     std::vector<vulkan::DescriptorSet> &descriptor_sets,
                                     vulkan::DescriptorSetLayout &&layout)
    : m_ctx(ctx), m_capacity(descriptor_sets.size()), m_next(0),
      m_layout(std::move(layout)), m_descriptor_sets(std::move(descriptor_sets)) {}

const vulkan::DescriptorSet SwapDescriptorSet::get() {
    auto &desc = m_descriptor_sets[m_next];
    m_next = ++m_next % m_capacity;
    return desc;
}
