#pragma once
#include "render_engine/CommandHandler.h"
#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/Fence.h"
#include "render_engine/FrameBuffer.h"
#include "render_engine/GraphicsPipeline.h"
#include "render_engine/RenderBody.h"
#include "render_engine/Semaphore.h"
#include "render_engine/TextPipeline.h"
#include "render_engine/Window.h"
#include "render_engine/fonts/Font.h"
#include "vulkan/vulkan_core.h"
#include <GLFW/glfw3.h>

using UniformBufferCollection = std::unique_ptr<std::vector<UniformBuffer>>;

class RenderEngine {
  private:
    bool framebuffer_resized = false;

    std::unique_ptr<Window> m_window;
    std::shared_ptr<CoreGraphicsContext> m_ctx;

    VkQueue m_graphics_queue;
    VkQueue m_present_queue;

    UniformBufferCollection m_window_dimension_buffers;
    std::unique_ptr<Semaphore> m_image_available_semaphore;
    std::unique_ptr<Semaphore> m_render_completed_semaphore;
    std::unique_ptr<Fence> m_in_flight_fence;

    std::unique_ptr<CommandHandler> m_command_handler;
    std::unique_ptr<SwapChain> m_swap_chain;
    VkRenderPass m_render_pass;
    std::unique_ptr<FrameBuffer> m_swap_chain_frame_buffer;

    std::unique_ptr<Sampler> m_sampler;
    std::unique_ptr<Texture> m_texture;

    VkDescriptorSetLayout m_geometry_descriptor_set_layout;
    std::unique_ptr<GraphicsPipeline> m_geometry_pipeline;

    std::unique_ptr<Font> m_font;
    VkDescriptorSetLayout m_text_descriptor_set_layout;
    std::unique_ptr<TextPipeline> m_text_pipeline;

    // TODO: Is this the way to go?
    struct {
        Window *window = nullptr;
        uint32_t image_index = 0;
        // TODO: Not actually optional but had to solve it like this for the construcors
        // sake. Also, should not command_buffer and command_buffer_wrapper be merged?
        std::optional<CommandBuffer> command_buffer_wrapper;
        VkCommandBuffer command_buffer = nullptr;
        VkSemaphore image_available_semaphore = nullptr;
        VkSemaphore signal_semaphore = nullptr;
        VkFence in_flight_fence = nullptr;
        SwapChain *swap_chain = nullptr;
    } m_current_render_pass;

    VkRenderPass create_render_pass(VkDevice &device, VkFormat &swapChainImageFormat);
    void recreate_swap_chain();

  public:
    RenderEngine(const uint32_t width, const uint32_t height, char const *title,
                 const UseFont use_font);
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
