#include "Semaphore.h"
#include <memory>

Semaphore::Semaphore(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                     const size_t size)
    : m_ctx(ctx), m_size(size), m_next(0) {
    m_semaphores.resize(size);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (auto i = 0; i < size; i++) {
        if (vkCreateSemaphore(ctx->logical_device, &semaphoreInfo, nullptr,
                              &m_semaphores[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create semaphore object for frame!");
        }
    }
}

Semaphore::~Semaphore() {
    if (m_ctx == nullptr) {
        return;
    }

    for (size_t i = 0; i < m_size; i++) {
        vkDestroySemaphore(m_ctx->logical_device, m_semaphores[i], nullptr);
    }
}

const VkSemaphore Semaphore::get() {
    auto &sem = m_semaphores[m_next];
    m_next = ++m_next % m_size;
    return sem;
}
