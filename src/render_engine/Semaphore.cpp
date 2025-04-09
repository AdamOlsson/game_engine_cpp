#include "Semaphore.h"
#include "render_engine/CoreGraphicsContext.h"
#include <memory>

Semaphore::Semaphore(std::shared_ptr<CoreGraphicsContext> ctx, const size_t size)
    : ctx(ctx), size(size), next(0) {
    semaphores.resize(size);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (auto i = 0; i < size; i++) {
        if (vkCreateSemaphore(ctx->device, &semaphoreInfo, nullptr, &semaphores[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create semaphore object for frame!");
        }
    }
}

Semaphore::~Semaphore() {
    for (size_t i = 0; i < size; i++) {
        vkDestroySemaphore(ctx->device, semaphores[i], nullptr);
    }
}

const VkSemaphore Semaphore::get() {
    auto &sem = semaphores[next];
    next = ++next % size;
    return sem;
}
