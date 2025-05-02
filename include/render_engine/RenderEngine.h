#pragma once
#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/GraphicsPipeline.h"
#include "render_engine/RenderBody.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/TextPipeline.h"
#include "render_engine/Window.h"
#include "render_engine/WindowConfig.h"
#include "render_engine/fonts/Font.h"
#include "vulkan/vulkan_core.h"
#include <GLFW/glfw3.h>

using UniformBufferCollection = std::unique_ptr<std::vector<UniformBuffer>>;

class RenderEngine {
  private:
    bool framebuffer_resized = false;
    Window m_window;
    std::shared_ptr<CoreGraphicsContext> m_ctx;

    DeviceQueues m_device_queues;
    UniformBufferCollection m_window_dimension_buffers;
    SwapChainManager m_swap_chain_manager;

    Sampler m_sampler;
    std::unique_ptr<Texture> m_texture; // Having this unique prevents a segfault

    VkDescriptorSetLayout m_geometry_descriptor_set_layout;
    std::unique_ptr<GraphicsPipeline> m_geometry_pipeline;

    std::unique_ptr<Font> m_font;
    VkDescriptorSetLayout m_text_descriptor_set_layout;
    std::unique_ptr<TextPipeline> m_text_pipeline;

    struct {
        CommandBuffer command_buffer;
    } m_current_render_pass;

  public:
    RenderEngine(const WindowConfig &window_config, const UseFont use_font);
    ~RenderEngine();

    bool should_window_close();

    /**
     * @brief Processes any events GLFW has queued. This function triggers any callback
     * registered. Currently only forwards the call to the Window class member.
     */
    void process_window_events();

    /**
     * @brief Register a callback function for mouse input events. Currently only forwards
     * the call to the Window class member.
     * @param cb The callback function which is triggered after a mouse input event.
     */
    void register_mouse_event_callback(MouseEventCallbackFn);

    void register_keyboard_event_callback(KeyboardEventCallbackFn);

    void render(const std::vector<std::reference_wrapper<const RenderBody>> &bodies);

    void render_text(const std::string &text, const glm::vec2 &location, const uint size);

    void wait_idle();

    bool begin_render_pass();

    bool end_render_pass();
};
