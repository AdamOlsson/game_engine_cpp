#include "Fence.h"
#include <memory>

vulkan::Fence::Fence(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                     const size_t size)
    : m_ctx(ctx), m_size(size), m_next(0), m_current(size) {
    m_fences.resize(size);

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (auto i = 0; i < size; i++) {
        if (vkCreateFence(ctx->logical_device, &fenceInfo, nullptr, &m_fences[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create fence object for frame!");
        }
    }
}

vulkan::Fence::~Fence() {
    if (m_ctx == nullptr) {
        return;
    }

    for (size_t i = 0; i < m_size; i++) {
        vkDestroyFence(m_ctx->logical_device, m_fences[i], nullptr);
    }
}

const VkFence &vulkan::Fence::next() {
    auto &sem = m_fences[m_next];
    m_current = m_next;
    m_next = ++m_next % m_size;
    return sem;
}
const VkFence &vulkan::Fence::current() {
    auto &sem = m_fences[m_current];
    return sem;
}
