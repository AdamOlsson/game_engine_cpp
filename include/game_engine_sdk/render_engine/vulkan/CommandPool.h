#pragma once

#include "game_engine_sdk/render_engine/SingleTimeCommandBuffer.h"
#include "game_engine_sdk/render_engine/graphics_context/GraphicsContext.h"
#include <memory>
namespace vulkan {
class CommandPool {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;
    VkCommandPool m_command_pool;

    VkCommandPool create_command_pool();

  public:
    CommandPool() = default;
    CommandPool(std::shared_ptr<graphics_context::GraphicsContext> ctx);
    ~CommandPool();

    CommandPool(CommandPool &&other) noexcept;
    CommandPool &operator=(CommandPool &&other) noexcept;

    CommandPool(const CommandPool &other) = delete;
    CommandPool &operator=(const CommandPool &other) = delete;

    operator VkCommandPool() const { return m_command_pool; }

    SingleTimeCommandBuffer get_single_time_command_buffer();
};

}; // namespace vulkan
