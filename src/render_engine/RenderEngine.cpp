#include "render_engine/RenderEngine.h"
#include "render_engine/DescriptorSetLayoutBuilder.h"
#include "render_engine/FrameBuffer.h"
#include "render_engine/RenderBody.h"
#include "render_engine/Window.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/resources/ResourceManager.h"
#include "render_engine/resources/images/ImageResource.h"
#include <memory>

UniformBufferCollection
create_uniform_buffers(std::shared_ptr<CoreGraphicsContext> &g_ctx, Window &window);

RenderEngine::RenderEngine(const uint32_t width, const uint32_t height, char const *title,
                           const UseFont use_font)
    : window(std::make_unique<Window>(width, height, title)),
      g_ctx(std::make_shared<CoreGraphicsContext>(true, window.get())),
      window_dimension_buffers(std::move(create_uniform_buffers(g_ctx, *window))),

      image_available_semaphore(std::make_unique<Semaphore>(g_ctx, MAX_FRAMES_IN_FLIGHT)),
      render_completed_semaphore(
          std::make_unique<Semaphore>(g_ctx, MAX_FRAMES_IN_FLIGHT)),
      in_flight_fence(std::make_unique<Fence>(g_ctx, MAX_FRAMES_IN_FLIGHT)),

      command_handler(std::make_unique<CommandHandler>(g_ctx, MAX_FRAMES_IN_FLIGHT)),
      swap_chain(std::make_unique<SwapChain>(g_ctx, window.get())),
      render_pass(create_render_pass(g_ctx->device, swap_chain->swapChainImageFormat)),
      swap_chain_frame_buffer(
          std::make_unique<FrameBuffer>(g_ctx, *swap_chain, render_pass)),
      sampler(std::make_unique<Sampler>(g_ctx)) {

    register_all_fonts();
    register_all_images();
    register_all_shaders();

    auto [_graphicsQueue, _presentQueue] = g_ctx->get_device_queues();
    graphics_queue = _graphicsQueue;
    present_queue = _presentQueue;

    auto descriptor_set_layout_builder =
        DescriptorSetLayoutBuilder()
            .add(StorageBuffer::createDescriptorSetLayoutBinding(0))
            .add(UniformBuffer::createDescriptorSetLayoutBinding(1))
            .add(Sampler::create_descriptor_set_layout_binding(2));

    VkCommandPool command_pool = command_handler->pool();

    auto &resource_manager = ResourceManager::get_instance();
    auto dog_image = resource_manager.get_resource<ImageResource>("DogImage");
    texture = std::make_unique<Texture>(g_ctx, command_pool, graphics_queue,
                                        dog_image->bytes(), dog_image->length());

    geometry_descriptor_set_layout = descriptor_set_layout_builder.build(g_ctx.get());
    geometry_pipeline = std::make_unique<GraphicsPipeline>(
        *window, g_ctx, *command_handler, *swap_chain, render_pass,
        *window_dimension_buffers, geometry_descriptor_set_layout, *sampler, *texture);

    switch (use_font) {
    case UseFont::Default: {
        auto default_font = resource_manager.get_resource<FontResource>("DefaultFont");
        font = std::make_unique<Font>(g_ctx, command_pool, graphics_queue, default_font);
        break;
    }
    default:
        font = nullptr;
        break;
    }

    if (font != nullptr) {
        text_descriptor_set_layout = descriptor_set_layout_builder.build(g_ctx.get());
        text_pipeline = std::make_unique<TextPipeline>(
            *window, g_ctx, *command_handler, *swap_chain, render_pass,
            *window_dimension_buffers, text_descriptor_set_layout, *sampler,
            *font->font_atlas);
    }
}

RenderEngine::~RenderEngine() {
    vkDestroyDescriptorSetLayout(g_ctx->device, geometry_descriptor_set_layout, nullptr);

    vkDestroyDescriptorSetLayout(g_ctx->device, text_descriptor_set_layout, nullptr);
    vkDestroyRenderPass(g_ctx->device, render_pass, nullptr);
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

    auto &command_buffer = current_render_pass.command_buffer;
    geometry_pipeline->render_circles(command_buffer, std::move(circle_instance_data));
    geometry_pipeline->render_triangles(command_buffer,
                                        std::move(triangle_instance_data));
    geometry_pipeline->render_rectangles(command_buffer,
                                         std::move(rectangle_instance_data));
    geometry_pipeline->render_hexagons(command_buffer, std::move(hexagon_instance_data));
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
                                font->encode_ascii_char(std::toupper(c))));
        count += 1.0f;
    }

    text_pipeline->render_text(current_render_pass.command_buffer,
                               std::move(instance_data));
}

void RenderEngine::wait_idle() { this->g_ctx->wait_idle(); }

bool RenderEngine::should_window_close() { return this->window->should_window_close(); }

void RenderEngine::process_window_events() { this->window->process_window_events(); }

void RenderEngine::register_mouse_event_callback(MouseEventCallbackFn cb) {
    this->window->register_mouse_event_callback(cb);
}

void RenderEngine::register_keyboard_event_callback(KeyboardEventCallbackFn cb) {
    this->window->register_keyboard_event_callback(cb);
}

VkRenderPass RenderEngine::create_render_pass(VkDevice &device,
                                              VkFormat &swapChainImageFormat) {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkRenderPass renderPass;
    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
    return renderPass;
}

void RenderEngine::recreate_swap_chain() {
    // All execution is paused when the window is minimized
    int width = 0, height = 0;
    glfwGetFramebufferSize(window->window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window->window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(g_ctx->device);
    swap_chain = std::make_unique<SwapChain>(g_ctx, window.get());
    swap_chain_frame_buffer =
        std::make_unique<FrameBuffer>(g_ctx, *swap_chain, render_pass);
}

UniformBufferCollection
create_uniform_buffers(std::shared_ptr<CoreGraphicsContext> &g_ctx, Window &window) {
    auto [width, height] = window.dimensions();
    UniformBufferObject ubo{
        .dimensions = glm::vec2(static_cast<float>(width), static_cast<float>(height))};
    auto uniform_buffers = std::make_unique<std::vector<UniformBuffer>>();
    uniform_buffers->reserve(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        uniform_buffers->emplace_back(g_ctx, sizeof(UniformBufferObject));
        uniform_buffers->at(i).updateUniformBuffer(ubo);
    }
    return std::move(uniform_buffers);
}

// TODO: begin_render_pass should instead return some struct instead of setting
// internals
bool RenderEngine::begin_render_pass() {
    const VkSemaphore _image_available_semaphore = image_available_semaphore->get();
    const VkSemaphore _signal_semaphore = render_completed_semaphore->get();
    const VkFence _in_flight_fence = in_flight_fence->get();

    vkWaitForFences(g_ctx->device, 1, &_in_flight_fence, VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result =
        vkAcquireNextImageKHR(g_ctx->device, swap_chain->swapChain, UINT64_MAX,
                              _image_available_semaphore, VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swap_chain();
        return false;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(g_ctx->device, 1, &_in_flight_fence);

    auto &command_buffer_wrapper = command_handler->buffer();
    const auto command_buffer = command_buffer_wrapper.begin();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = render_pass;
    renderPassInfo.framebuffer = swap_chain_frame_buffer->get();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swap_chain->swapChainExtent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swap_chain->swapChainExtent.width);
    viewport.height = static_cast<float>(swap_chain->swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swap_chain->swapChainExtent;
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    current_render_pass.window = window.get();
    current_render_pass.image_index = imageIndex;
    current_render_pass.command_buffer_wrapper = command_buffer_wrapper;
    current_render_pass.command_buffer = command_buffer;
    current_render_pass.image_available_semaphore = _image_available_semaphore;
    current_render_pass.signal_semaphore = _signal_semaphore;
    current_render_pass.in_flight_fence = _in_flight_fence;
    current_render_pass.swap_chain = swap_chain.get();

    return true;
}

bool RenderEngine::end_render_pass() {
    auto *window = current_render_pass.window;
    const auto image_index = current_render_pass.image_index;
    auto &command_buffer_wrapper = current_render_pass.command_buffer_wrapper;
    const auto &command_buffer = current_render_pass.command_buffer;
    const auto &image_available_semaphore = current_render_pass.image_available_semaphore;
    const auto &signal_semaphore = current_render_pass.signal_semaphore;
    const auto &in_flight_fence = current_render_pass.in_flight_fence;
    const auto swap_chain = current_render_pass.swap_chain;

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

    if (vkQueueSubmit(graphics_queue, 1, &submitInfo, in_flight_fence) != VK_SUCCESS) {
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

    VkResult result = vkQueuePresentKHR(present_queue, &presentInfo);

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

    /*currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;*/
    current_render_pass.command_buffer_wrapper = std::nullopt;
    return true;
}
