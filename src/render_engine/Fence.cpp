#include "Fence.h"
#include "render_engine/CoreGraphicsContext.h"
#include <memory>

Fence::Fence(std::shared_ptr<CoreGraphicsContext> ctx, const size_t size)
    : ctx(ctx), size(size), next(0) {
    fences.resize(size);

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (auto i = 0; i < size; i++) {
        if (vkCreateFence(ctx->device, &fenceInfo, nullptr, &fences[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create fence object for frame!");
        }
    }
}

Fence::~Fence() {
    for (size_t i = 0; i < size; i++) {
        vkDestroyFence(ctx->device, fences[i], nullptr);
    }
}

const VkFence Fence::get() {
    auto &sem = fences[next];
    next = ++next % size;
    return sem;
}
