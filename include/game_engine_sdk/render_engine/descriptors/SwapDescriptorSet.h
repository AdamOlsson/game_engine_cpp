#pragma once

#include "game_engine_sdk/render_engine/vulkan/DescriptorSet.h"
#include "game_engine_sdk/render_engine/vulkan/DescriptorSetLayout.h"
#include "game_engine_sdk/render_engine/vulkan/GraphicsContext.h"
#include <cstddef>

class SwapDescriptorSet {
    friend class SwapDescriptorSetBuilder;

  private:
    std::shared_ptr<vulkan::GraphicsContext> m_ctx;
    size_t m_capacity;
    size_t m_next;

    vulkan::DescriptorSetLayout m_layout;
    std::vector<vulkan::DescriptorSet> m_descriptor_sets;

    SwapDescriptorSet(std::shared_ptr<vulkan::GraphicsContext> ctx,
                      std::vector<vulkan::DescriptorSet> &descriptor_sets,
                      vulkan::DescriptorSetLayout &&layout);

  public:
    SwapDescriptorSet() = default;

    SwapDescriptorSet(SwapDescriptorSet &&other) noexcept = default;
    SwapDescriptorSet &operator=(SwapDescriptorSet &&other) noexcept = default;
    SwapDescriptorSet(const SwapDescriptorSet &other) = delete;
    SwapDescriptorSet &operator=(const SwapDescriptorSet &other) = delete;

    ~SwapDescriptorSet() = default;

    vulkan::DescriptorSetLayout &get_layout() { return m_layout; }
    const vulkan::DescriptorSet get();
};
