#pragma once
#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/GeometryPipeline.h"
#include "render_engine/RenderBody.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/Window.h"
#include "render_engine/WindowConfig.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/ui/ElementProperties.h"
#include "render_engine/ui/State.h"
#include "render_engine/ui/TextPipeline.h"
#include "render_engine/ui/UIPipeline.h"
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
    std::unique_ptr<GeometryPipeline> m_geometry_pipeline;

    std::unique_ptr<Font> m_font;

    std::unique_ptr<ui::TextPipeline> m_text_pipeline;
    std::unique_ptr<ui::UIPipeline> m_ui_pipeline;

    struct {
        CommandBuffer command_buffer;
    } m_current_render_pass;

    void text_kerning(const font::KerningMap &kerning_map, const std::string_view text,
                      const ui::ElementProperties properties);

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

    void render_text(const ui::TextBox &text_box);

    void render_ui(const ui::State &state);

    void wait_idle();

    bool begin_render_pass();

    bool end_render_pass();
};
