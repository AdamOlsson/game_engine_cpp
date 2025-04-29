#include "render_engine/RenderEngine.h"
#include "render_engine/DescriptorSetLayoutBuilder.h"
#include "render_engine/GraphicsPipeline.h"
#include "render_engine/RenderBody.h"
#include "render_engine/SwapChain.h"
#include "render_engine/Window.h"
#include "render_engine/WindowConfig.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/resources/ResourceManager.h"
#include "render_engine/resources/images/ImageResource.h"
#include "vulkan/vulkan_core.h"
#include <memory>

// TODO: Fix the todos in the begin_render_pass() function
// TODO: Texture should only have a certain "level" of classes, what I mean is in short
//       that any Vulkan classes should not be exposed. This also goes for the
//       RenderEngine
// TODO: How could we improve the API of the Font constructor
// TODO: How could we improve the API of the TextPipeline constructor
// TODO: Do the same evaluation done for the RenderEngine on the GameEngine

UniformBufferCollection create_uniform_buffers(std::shared_ptr<CoreGraphicsContext> &ctx,
                                               Window &window);

RenderEngine::RenderEngine(const WindowConfig &window_config, const UseFont use_font)
    : m_window(Window(window_config)),
      m_ctx(std::make_shared<CoreGraphicsContext>(true, m_window)),
      m_window_dimension_buffers(std::move(create_uniform_buffers(m_ctx, m_window))),

      m_image_available_semaphore(Semaphore(m_ctx, MAX_FRAMES_IN_FLIGHT)),
      m_render_completed_semaphore(Semaphore(m_ctx, MAX_FRAMES_IN_FLIGHT)),
      m_in_flight_fence(Fence(m_ctx, MAX_FRAMES_IN_FLIGHT)),

      m_command_handler(CommandHandler(m_ctx, MAX_FRAMES_IN_FLIGHT)),
      m_swap_chain(SwapChain(m_ctx, m_window)), m_sampler(Sampler(m_ctx)) {

    register_all_fonts();
    register_all_images();
    register_all_shaders();

    m_device_queues = m_ctx->get_device_queues();

    auto descriptor_set_layout_builder =
        DescriptorSetLayoutBuilder()
            .add(StorageBuffer::createDescriptorSetLayoutBinding(0))
            .add(UniformBuffer::createDescriptorSetLayoutBinding(1))
            .add(Sampler::create_descriptor_set_layout_binding(2));

    CommandPool command_pool = m_command_handler.pool();

    auto &resource_manager = ResourceManager::get_instance();
    auto dog_image = resource_manager.get_resource<ImageResource>("DogImage");
    m_texture = Texture::unique_from_image_resource(
        m_ctx, command_pool, m_device_queues.graphics_queue, dog_image);

    m_geometry_descriptor_set_layout = descriptor_set_layout_builder.build(m_ctx.get());
    m_geometry_pipeline = std::make_unique<GraphicsPipeline>(
        m_window, m_ctx, m_command_handler, m_swap_chain, m_swap_chain.m_render_pass,
        *m_window_dimension_buffers, m_geometry_descriptor_set_layout, m_sampler,
        *m_texture);

    switch (use_font) {
    case UseFont::Default: {
        auto default_font = resource_manager.get_resource<FontResource>("DefaultFont");
        m_font = std::make_unique<Font>(m_ctx, command_pool,
                                        m_device_queues.graphics_queue, default_font);
        break;
    }
    default:
        m_font = nullptr;
        break;
    }

    if (m_font != nullptr) {
        m_text_descriptor_set_layout = descriptor_set_layout_builder.build(m_ctx.get());
        m_text_pipeline = std::make_unique<TextPipeline>(
            m_window, m_ctx, m_command_handler, m_swap_chain, m_swap_chain.m_render_pass,
            *m_window_dimension_buffers, m_text_descriptor_set_layout, m_sampler,
            *m_font->font_atlas);
    }
}

RenderEngine::~RenderEngine() {
    vkDestroyDescriptorSetLayout(m_ctx->device, m_geometry_descriptor_set_layout,
                                 nullptr);
    vkDestroyDescriptorSetLayout(m_ctx->device, m_text_descriptor_set_layout, nullptr);
}

void RenderEngine::render(
    const std::vector<std::reference_wrapper<const RenderBody>> &bodies) {
    std::vector<StorageBufferObject> circle_instance_data = {};
    std::vector<StorageBufferObject> triangle_instance_data = {};
    std::vector<StorageBufferObject> rectangle_instance_data = {};
    std::vector<StorageBufferObject> hexagon_instance_data = {};
    std::vector<StorageBufferObject> arrow_instance_data = {};
    std::vector<StorageBufferObject> line_instance_data = {};
    for (auto b : bodies) {
        auto deref_b = b.get();
        switch (deref_b.shape.encode_shape_type()) {
        case ShapeTypeEncoding::CircleShape:
            circle_instance_data.push_back(
                StorageBufferObject(deref_b.position, deref_b.color, deref_b.rotation,
                                    deref_b.shape, deref_b.uvwt));
            break;
        case ShapeTypeEncoding::TriangleShape:
            triangle_instance_data.push_back(
                StorageBufferObject(deref_b.position, deref_b.color, deref_b.rotation,
                                    deref_b.shape, deref_b.uvwt));
            break;
        case ShapeTypeEncoding::RectangleShape:
            rectangle_instance_data.push_back(
                StorageBufferObject(deref_b.position, deref_b.color, deref_b.rotation,
                                    deref_b.shape, deref_b.uvwt));
            break;
        case ShapeTypeEncoding::HexagonShape:
            hexagon_instance_data.push_back(
                StorageBufferObject(deref_b.position, deref_b.color, deref_b.rotation,
                                    deref_b.shape, deref_b.uvwt));
            break;
        case ShapeTypeEncoding::ArrowShape:
            arrow_instance_data.push_back(
                StorageBufferObject(deref_b.position, deref_b.color, deref_b.rotation,
                                    deref_b.shape, deref_b.uvwt));
            break;
        case ShapeTypeEncoding::LineShape:
            line_instance_data.push_back(
                StorageBufferObject(deref_b.position, deref_b.color, deref_b.rotation,
                                    deref_b.shape, deref_b.uvwt));
            break;
        default:
            break;
        };
    }

    auto &command_buffer = m_current_render_pass.command_buffer;
    m_geometry_pipeline->render_circles(command_buffer, std::move(circle_instance_data));
    m_geometry_pipeline->render_triangles(command_buffer,
                                          std::move(triangle_instance_data));
    m_geometry_pipeline->render_rectangles(command_buffer,
                                           std::move(rectangle_instance_data));
    m_geometry_pipeline->render_hexagons(command_buffer,
                                         std::move(hexagon_instance_data));
}

void RenderEngine::render_text(const std::string &text, const glm::vec2 &location,
                               const uint size) {

    // TODO: At somepoint we use a separate struct for text instance data
    // TODO: We can probably move all this computations to the shaders
    std::vector<StorageBufferObject> instance_data = {};
    const glm::vec3 offset(size / 2.5, 0.0f, 0.0f);
    const glm::vec3 loc(location.x, location.y, 0.0f);
    float count = 0;
    for (const char &c : text) {

        instance_data.push_back(
            StorageBufferObject(loc + offset * count, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f,
                                Shape::create_rectangle_data(size, size),
                                m_font->encode_ascii_char(std::toupper(c))));
        count += 1.0f;
    }

    m_text_pipeline->render_text(m_current_render_pass.command_buffer,
                                 std::move(instance_data));
}

void RenderEngine::wait_idle() { m_ctx->wait_idle(); }

bool RenderEngine::should_window_close() { return m_window.should_window_close(); }

void RenderEngine::process_window_events() { m_window.process_window_events(); }

void RenderEngine::register_mouse_event_callback(MouseEventCallbackFn cb) {
    m_window.register_mouse_event_callback(cb);
}

void RenderEngine::register_keyboard_event_callback(KeyboardEventCallbackFn cb) {
    m_window.register_keyboard_event_callback(cb);
}

void RenderEngine::recreate_swap_chain() {
    // All execution is paused when the window is minimized
    int width = 0, height = 0;
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_window.window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_ctx->device);
    m_swap_chain = SwapChain(m_ctx, m_window);
}

UniformBufferCollection create_uniform_buffers(std::shared_ptr<CoreGraphicsContext> &ctx,
                                               Window &window) {
    auto [width, height] = window.dimensions();
    UniformBufferObject ubo{
        .dimensions = glm::vec2(static_cast<float>(width), static_cast<float>(height))};
    auto uniform_buffers = std::make_unique<std::vector<UniformBuffer>>();
    uniform_buffers->reserve(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        uniform_buffers->emplace_back(ctx, sizeof(UniformBufferObject));
        uniform_buffers->at(i).updateUniformBuffer(ubo);
    }
    return std::move(uniform_buffers);
}

std::optional<uint32_t> RenderEngine::get_next_image_index(VkSemaphore &image_available) {
    // TODO: Can this be moved to the SwapChain class?
    uint32_t image_index;
    VkResult result =
        vkAcquireNextImageKHR(m_ctx->device, m_swap_chain.m_swap_chain, UINT64_MAX,
                              image_available, VK_NULL_HANDLE, &image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swap_chain();
        return std::nullopt;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    return image_index;
}

void RenderEngine::set_viewport(const VkCommandBuffer &command_buffer,
                                const Dimension &dim) {
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swap_chain.m_extent.width);
    viewport.height = static_cast<float>(m_swap_chain.m_extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);
}

void RenderEngine::set_scissor(const VkCommandBuffer &command_buffer,
                               const VkExtent2D &extent) {
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swap_chain.m_extent;
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void RenderEngine::begin_render_pass_(const VkCommandBuffer &command_buffer,
                                      SwapChain &swap_chain) {
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swap_chain.m_render_pass;
    renderPassInfo.framebuffer = swap_chain.get_frame_buffer();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swap_chain.m_extent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

bool RenderEngine::begin_render_pass() {

    const VkFence _in_flight_fence = m_in_flight_fence.get();
    vkWaitForFences(m_ctx->device, 1, &_in_flight_fence, VK_TRUE, UINT64_MAX);

    VkSemaphore _image_available_semaphore = m_image_available_semaphore.get();
    auto imageIndex_ = get_next_image_index(_image_available_semaphore);
    if (!imageIndex_.has_value()) {
        return false;
    }
    auto imageIndex = imageIndex_.value();

    // TODO: Refactor CommandHandler class to CommandBuffer
    //      - Obfuscate the command pool in the command buffer class
    //      - Merge the use of the CommandHandler(Buffer) with
    //        begin_single_time_commands()
    // TODO: This wrapper syntax is awful, would be improved after above todo.
    auto &command_buffer_wrapper = m_command_handler.buffer();
    const auto command_buffer = command_buffer_wrapper.begin();

    begin_render_pass_(command_buffer, m_swap_chain);
    set_viewport(command_buffer, Dimension::from_extent2d(m_swap_chain.m_extent));
    set_scissor(command_buffer, m_swap_chain.m_extent);

    vkResetFences(m_ctx->device, 1, &_in_flight_fence);

    // TODO: We should finally make this nice, could this be a RenderPass class?
    m_current_render_pass.window = &m_window;
    m_current_render_pass.image_index = imageIndex;
    m_current_render_pass.command_buffer_wrapper = command_buffer_wrapper;
    m_current_render_pass.command_buffer = command_buffer;
    m_current_render_pass.image_available_semaphore = _image_available_semaphore;
    m_current_render_pass.in_flight_fence = _in_flight_fence;
    m_current_render_pass.swap_chain = &m_swap_chain;

    return true;
}

void RenderEngine::submit_render_pass(const VkCommandBuffer &command_buffer,
                                      const VkSemaphore &image_available,
                                      const VkSemaphore &submit_completed,
                                      const VkFence &in_flight) {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pCommandBuffers = &command_buffer;

    VkSemaphore waitSemaphores[] = {image_available};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;

    VkSemaphore signalSemaphores[] = {submit_completed};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_device_queues.graphics_queue, 1, &submitInfo, in_flight) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
}

VkResult RenderEngine::present_render_pass(SwapChain &swap_chain,
                                           const VkQueue &present_queue,
                                           const VkSemaphore &submit_completed,
                                           const uint32_t image_index) {
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &submit_completed;

    VkSwapchainKHR swapChains[] = {swap_chain.m_swap_chain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &image_index;
    presentInfo.pResults = nullptr; // Optional

    VkResult result = vkQueuePresentKHR(present_queue, &presentInfo);
    return result;
}

bool RenderEngine::end_render_pass() {
    auto *window = m_current_render_pass.window;
    const auto image_index = m_current_render_pass.image_index;
    auto &command_buffer_wrapper = m_current_render_pass.command_buffer_wrapper;
    const auto &command_buffer = m_current_render_pass.command_buffer;
    const auto &image_available_semaphore =
        m_current_render_pass.image_available_semaphore;
    const auto &in_flight_fence = m_current_render_pass.in_flight_fence;
    const auto swap_chain = m_current_render_pass.swap_chain;

    vkCmdEndRenderPass(command_buffer);
    command_buffer_wrapper->end();

    // If we make the command buffer a pure OOP object, this semaphore can be internal
    const VkSemaphore signal_semaphore = m_render_completed_semaphore.get();
    submit_render_pass(command_buffer, image_available_semaphore, signal_semaphore,
                       in_flight_fence);

    VkResult result = present_render_pass(m_swap_chain, m_device_queues.present_queue,
                                          signal_semaphore, image_index);

    // TODO: framebufferResized needs to be set from the window callback when the
    // window is resized
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        framebuffer_resized) {
        recreate_swap_chain();

        return false;
        // It is important to do this after vkQueuePresentKHR to ensure that the
        // semaphores are in a consistent state, otherwise a signaled semaphore
        // may never be properly waited upon.
        framebuffer_resized = false;
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    m_current_render_pass.command_buffer_wrapper = std::nullopt;
    return true;
}
