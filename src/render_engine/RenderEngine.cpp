#include "render_engine/RenderEngine.h"
#include "render_engine/DescriptorSetLayoutBuilder.h"
#include "render_engine/FrameBuffer.h"
#include "render_engine/GraphicsPipeline.h"
#include "render_engine/RenderBody.h"
#include "render_engine/SwapChain.h"
#include "render_engine/Window.h"
#include "render_engine/WindowConfig.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/resources/ResourceManager.h"
#include "render_engine/resources/images/ImageResource.h"
#include <memory>

UniformBufferCollection create_uniform_buffers(std::shared_ptr<CoreGraphicsContext> &ctx,
                                               Window &window);

RenderEngine::RenderEngine(const WindowConfig &window_config, const UseFont use_font)
    : m_window(Window(window_config)),
      m_ctx(std::make_shared<CoreGraphicsContext>(true, m_window)),
      m_window_dimension_buffers(std::move(create_uniform_buffers(m_ctx, m_window))),

      m_image_available_semaphore(Semaphore(m_ctx, MAX_FRAMES_IN_FLIGHT)),
      m_render_completed_semaphore(Semaphore(m_ctx, MAX_FRAMES_IN_FLIGHT)),
      m_in_flight_fence(Fence(m_ctx, MAX_FRAMES_IN_FLIGHT)),

      m_command_handler(std::make_unique<CommandHandler>(m_ctx, MAX_FRAMES_IN_FLIGHT)),
      m_swap_chain(std::make_unique<SwapChain>(m_ctx, m_window)),
      m_render_pass(create_render_pass(m_ctx.get(), m_swap_chain.get())),
      m_swap_chain_frame_buffer(
          std::make_unique<FrameBuffer>(m_ctx, *m_swap_chain, m_render_pass)),
      m_sampler(std::make_unique<Sampler>(m_ctx)) {

    register_all_fonts();
    register_all_images();
    register_all_shaders();

    m_device_queues = m_ctx->get_device_queues();

    auto descriptor_set_layout_builder =
        DescriptorSetLayoutBuilder()
            .add(StorageBuffer::createDescriptorSetLayoutBinding(0))
            .add(UniformBuffer::createDescriptorSetLayoutBinding(1))
            .add(Sampler::create_descriptor_set_layout_binding(2));

    VkCommandPool command_pool = m_command_handler->pool();

    auto &resource_manager = ResourceManager::get_instance();
    auto dog_image = resource_manager.get_resource<ImageResource>("DogImage");
    m_texture =
        std::make_unique<Texture>(m_ctx, command_pool, m_device_queues.graphics_queue,
                                  dog_image->bytes(), dog_image->length());

    m_geometry_descriptor_set_layout = descriptor_set_layout_builder.build(m_ctx.get());
    m_geometry_pipeline = std::make_unique<GraphicsPipeline>(
        m_window, m_ctx, *m_command_handler, *m_swap_chain, m_render_pass,
        *m_window_dimension_buffers, m_geometry_descriptor_set_layout, *m_sampler,
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
            m_window, m_ctx, *m_command_handler, *m_swap_chain, m_render_pass,
            *m_window_dimension_buffers, m_text_descriptor_set_layout, *m_sampler,
            *m_font->font_atlas);
    }
}

RenderEngine::~RenderEngine() {
    vkDestroyDescriptorSetLayout(m_ctx->device, m_geometry_descriptor_set_layout,
                                 nullptr);

    vkDestroyDescriptorSetLayout(m_ctx->device, m_text_descriptor_set_layout, nullptr);
    vkDestroyRenderPass(m_ctx->device, m_render_pass, nullptr);
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

VkRenderPass RenderEngine::create_render_pass(CoreGraphicsContext *ctx,
                                              SwapChain *swap_chain) {
    VkAttachmentDescription color_attachment{};
    color_attachment.format = swap_chain->swapChainImageFormat;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    VkRenderPass render_pass;
    if (vkCreateRenderPass(ctx->device, &render_pass_info, nullptr, &render_pass) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
    return render_pass;
}

void RenderEngine::recreate_swap_chain() {
    // All execution is paused when the window is minimized
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_window.window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_window.window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_ctx->device);
    m_swap_chain = std::make_unique<SwapChain>(m_ctx, m_window);
    m_swap_chain_frame_buffer =
        std::make_unique<FrameBuffer>(m_ctx, *m_swap_chain, m_render_pass);
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

// TODO: begin_render_pass should instead return some struct instead of setting
// internals
bool RenderEngine::begin_render_pass() {
    const VkSemaphore _image_available_semaphore = m_image_available_semaphore.get();
    const VkSemaphore _signal_semaphore = m_render_completed_semaphore.get();
    const VkFence _in_flight_fence = m_in_flight_fence.get();

    vkWaitForFences(m_ctx->device, 1, &_in_flight_fence, VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result =
        vkAcquireNextImageKHR(m_ctx->device, m_swap_chain->swapChain, UINT64_MAX,
                              _image_available_semaphore, VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swap_chain();
        return false;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(m_ctx->device, 1, &_in_flight_fence);

    auto &command_buffer_wrapper = m_command_handler->buffer();
    const auto command_buffer = command_buffer_wrapper.begin();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_render_pass;
    renderPassInfo.framebuffer = m_swap_chain_frame_buffer->get();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_swap_chain->swapChainExtent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swap_chain->swapChainExtent.width);
    viewport.height = static_cast<float>(m_swap_chain->swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swap_chain->swapChainExtent;
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    m_current_render_pass.window = &m_window;
    m_current_render_pass.image_index = imageIndex;
    m_current_render_pass.command_buffer_wrapper = command_buffer_wrapper;
    m_current_render_pass.command_buffer = command_buffer;
    m_current_render_pass.image_available_semaphore = _image_available_semaphore;
    m_current_render_pass.signal_semaphore = _signal_semaphore;
    m_current_render_pass.in_flight_fence = _in_flight_fence;
    m_current_render_pass.swap_chain = m_swap_chain.get();

    return true;
}

bool RenderEngine::end_render_pass() {
    auto *window = m_current_render_pass.window;
    const auto image_index = m_current_render_pass.image_index;
    auto &command_buffer_wrapper = m_current_render_pass.command_buffer_wrapper;
    const auto &command_buffer = m_current_render_pass.command_buffer;
    const auto &image_available_semaphore =
        m_current_render_pass.image_available_semaphore;
    const auto &signal_semaphore = m_current_render_pass.signal_semaphore;
    const auto &in_flight_fence = m_current_render_pass.in_flight_fence;
    const auto swap_chain = m_current_render_pass.swap_chain;

    vkCmdEndRenderPass(command_buffer);
    command_buffer_wrapper->end();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pCommandBuffers = &command_buffer;

    VkSemaphore waitSemaphores[] = {image_available_semaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;

    VkSemaphore signalSemaphores[] = {signal_semaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_device_queues.graphics_queue, 1, &submitInfo, in_flight_fence) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swap_chain->swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &image_index;
    presentInfo.pResults = nullptr; // Optional

    VkResult result = vkQueuePresentKHR(m_device_queues.present_queue, &presentInfo);

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
