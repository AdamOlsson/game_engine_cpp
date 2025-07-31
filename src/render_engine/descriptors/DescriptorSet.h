#pragma once

#include "render_engine/descriptors/DescriptorSetLayout.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include <cstddef>

class DescriptorSet {
    friend class DescriptorSetBuilder;

  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;
    size_t m_capacity;
    size_t m_next;

    DescriptorSetLayout m_layout;
    std::vector<VkDescriptorSet> m_descriptor_sets;

    DescriptorSet(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                  std::vector<VkDescriptorSet> &descriptor_sets,
                  DescriptorSetLayout &&layout);

  public:
    DescriptorSet() = default;

    DescriptorSet(DescriptorSet &&other) noexcept = default;
    DescriptorSet(const DescriptorSet &other) = delete;
    DescriptorSet &operator=(DescriptorSet &&other) noexcept = default;
    DescriptorSet &operator=(const DescriptorSet &other) = delete;

    ~DescriptorSet() = default;

    DescriptorSetLayout &get_layout() { return m_layout; }
    const VkDescriptorSet get();
};
