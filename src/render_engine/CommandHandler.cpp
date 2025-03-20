#include "CommandHandler.h"
#include "util.h"
#include "vulkan/vulkan_core.h"

VkCommandPool create_command_pool(const CoreGraphicsContext *ctx);

std::vector<CommandBuffer> create_command_buffers(const VkDevice &device,
                                                  const VkCommandPool &command_pool,
                                                  const int capacity);

CommandBuffer::CommandBuffer(VkCommandBuffer &command_buffer)
    : command_buffer(command_buffer) {}

VkCommandBuffer CommandBuffer::begin() {
    vkResetCommandBuffer(command_buffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                  // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional
    if (vkBeginCommandBuffer(command_buffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    return command_buffer;
}

void CommandBuffer::end() {
    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

CommandHandler::CommandHandler(std::shared_ptr<CoreGraphicsContext> ctx,
                               const size_t num_buffers)
    : ctx(ctx), num_buffers(num_buffers), command_pool(create_command_pool(ctx.get())),
      command_buffers(create_command_buffers(ctx->device, command_pool, num_buffers)) {}

CommandHandler::~CommandHandler() {
    vkDestroyCommandPool(ctx->device, command_pool, nullptr);
}

VkCommandPool CommandHandler::pool() { return command_pool; }

CommandBuffer &CommandHandler::buffer() {
    auto &buf = command_buffers[next_buffer];
    next_buffer = ++next_buffer % num_buffers;
    return buf;
}

VkCommandPool create_command_pool(const CoreGraphicsContext *ctx) {
    QueueFamilyIndices queueFamilyIndices =
        findQueueFamilies(ctx->physicalDevice, ctx->surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    VkCommandPool commandPool;
    if (vkCreateCommandPool(ctx->device, &poolInfo, nullptr, &commandPool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
    return commandPool;
}

std::vector<CommandBuffer> create_command_buffers(const VkDevice &device,
                                                  const VkCommandPool &command_pool,
                                                  const int capacity) {
    std::vector<VkCommandBuffer> commandBuffers;
    commandBuffers.resize(capacity);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = command_pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    std::vector<CommandBuffer> wrapped_buffers;
    wrapped_buffers.reserve(capacity);
    for (auto b : commandBuffers) {
        wrapped_buffers.push_back(CommandBuffer(b));
    }
    return wrapped_buffers;
}
