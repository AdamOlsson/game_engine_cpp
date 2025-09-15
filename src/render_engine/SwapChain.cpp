#include "render_engine/SwapChain.h"
#include "vulkan/vulkan_core.h"

SwapChain::SwapChain() : m_swap_chain(VK_NULL_HANDLE), m_render_pass(VK_NULL_HANDLE) {}

SwapChain::SwapChain(std::shared_ptr<graphics_context::GraphicsContext> ctx)
    : m_ctx(ctx), m_next_frame_buffer(0) {

    VkSwapchainKHR old_swap_chain = VK_NULL_HANDLE;
    setup(old_swap_chain);
}

SwapChain::SwapChain(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                     SwapChain &old)
    : m_ctx(ctx), m_next_frame_buffer(old.m_next_frame_buffer) {
    VkSwapchainKHR old_swap_chain = old.m_swap_chain;
    setup(old_swap_chain);
}

void SwapChain::recreate_swap_chain() {
    // All execution is paused when the window is minimized
    // clang-format off
    while (m_ctx->window->is_minimized()) {}
    // clang-format on 

    m_ctx->wait_idle();

    VkRenderPass old_render_pass = m_render_pass;
    VkSwapchainKHR old_swap_chain = m_swap_chain;
    /*std::vector<VkFramebuffer> old_frame_buffers = m_frame_buffers;*/
    setup(old_swap_chain);

    vkDestroyRenderPass(m_ctx->logical_device, old_render_pass, nullptr);
    vkDestroySwapchainKHR(m_ctx->logical_device, old_swap_chain, nullptr);
    /*for (size_t i = 0; i < m_frame_buffers.size(); i++) {*/
    /*    vkDestroyFramebuffer(m_ctx->logical_device, old_frame_buffers[i], nullptr);*/
    /*}*/
}

void SwapChain::setup(VkSwapchainKHR &old_swap_chain) {
    vulkan::device::SwapChainSupportDetails swap_chain_support =
        m_ctx->physical_device.query_swap_chain_support(m_ctx->surface);

    uint32_t image_count = swap_chain_support.get_image_count();

    VkSurfaceFormatKHR surface_format = swap_chain_support.choose_swap_surface_format(
        {.format = VK_FORMAT_B8G8R8A8_SRGB,
         .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR});

    auto window_framebuffer_size = m_ctx->window->get_framebuffer_size<uint32_t>();
    m_extent = swap_chain_support.choose_swap_extent(window_framebuffer_size);

    m_swap_chain = create_swap_chain(image_count, surface_format, swap_chain_support,
                                     old_swap_chain);

    m_images = create_swap_chain_images(image_count);
    m_image_views = create_image_views(surface_format.format);
    m_render_pass = create_render_pass(surface_format.format);
    m_framebuffers = create_framebuffers();
}

void SwapChain::destroy() {
    vkDestroyRenderPass(m_ctx->logical_device, m_render_pass, nullptr);
    vkDestroySwapchainKHR(m_ctx->logical_device, m_swap_chain, nullptr);
}

SwapChain::~SwapChain() {
    if (m_swap_chain == VK_NULL_HANDLE) {
        return;
    }
    destroy();
    m_swap_chain = VK_NULL_HANDLE;
}

SwapChain::SwapChain(SwapChain &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), m_next_frame_buffer(other.m_next_frame_buffer),
      m_images(std::move(other.m_images)), m_image_views(std::move(other.m_image_views)),
      m_framebuffers(std::move(other.m_framebuffers)), m_swap_chain(other.m_swap_chain),
      m_extent(other.m_extent), m_render_pass(other.m_render_pass) {
    other.m_swap_chain = VK_NULL_HANDLE;
    other.m_render_pass = VK_NULL_HANDLE;
}

SwapChain &SwapChain::operator=(SwapChain &&other) noexcept {
    if (this != &other) {
        if (m_swap_chain != VK_NULL_HANDLE) {
            destroy();
        }

        m_ctx = std::move(other.m_ctx);
        m_next_frame_buffer = other.m_next_frame_buffer;
        m_images = std::move(other.m_images);
        m_image_views = std::move(other.m_image_views);
        m_framebuffers = std::move(other.m_framebuffers);
        m_swap_chain = other.m_swap_chain;
        m_extent = other.m_extent;
        m_render_pass = other.m_render_pass;

        other.m_swap_chain = VK_NULL_HANDLE;
        other.m_render_pass = VK_NULL_HANDLE;
    }
    return *this;
}

VkSwapchainKHR
SwapChain::create_swap_chain(uint32_t image_count, VkSurfaceFormatKHR &surface_format,
                             vulkan::device::SwapChainSupportDetails &swap_chain_support,
                             VkSwapchainKHR &old_swap_chain) {
    VkPresentModeKHR present_mode =
        swap_chain_support.choose_swap_present_mode(VK_PRESENT_MODE_MAILBOX_KHR);

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = m_ctx->surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = m_extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    vulkan::device::QueueFamilyIndices indices =
        m_ctx->physical_device.find_queue_families(m_ctx->surface);

    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                     indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;     // Optional
        create_info.pQueueFamilyIndices = nullptr; // Optional
    }

    create_info.preTransform = swap_chain_support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = old_swap_chain;
    VkSwapchainKHR swap_chain;
    if (vkCreateSwapchainKHR(m_ctx->logical_device, &create_info, nullptr, &swap_chain) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }
    return swap_chain;
}

std::vector<VkImage> SwapChain::create_swap_chain_images(uint32_t image_count) {
    std::vector<VkImage> images;
    images.resize(image_count);
    vkGetSwapchainImagesKHR(m_ctx->logical_device, m_swap_chain, &image_count,
                            images.data());
    return images;
}

std::vector<vulkan::ImageView> SwapChain::create_image_views(VkFormat &image_format) {
    std::vector<vulkan::ImageView> swap_chain_image_views;
    swap_chain_image_views.reserve(m_images.size());
    for (size_t i = 0; i < m_images.size(); i++) {
        swap_chain_image_views.emplace_back(m_ctx, m_images[i], image_format);
    }
    return swap_chain_image_views;
}

std::vector<vulkan::Framebuffer> SwapChain::create_framebuffers() {

    const size_t capacity = m_image_views.size();
    std::vector<vulkan::Framebuffer> swap_chain_framebuffers;
    swap_chain_framebuffers.reserve(capacity);

    for (size_t i = 0; i < capacity; i++) {
        swap_chain_framebuffers.emplace_back(m_ctx, m_image_views[i], m_render_pass, m_extent);
    }
    return swap_chain_framebuffers;
}

VkRenderPass SwapChain::create_render_pass(VkFormat &image_format) {
    VkAttachmentDescription color_attachment{};
    color_attachment.format = image_format;
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
    if (vkCreateRenderPass(m_ctx->logical_device, &render_pass_info, nullptr,
                           &render_pass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
    return render_pass;
}

VkFramebuffer SwapChain::get_framebuffer(uint32_t image_index) {
    if (image_index >= m_framebuffers.size()) {
        throw std::runtime_error("Image index out of range for framebuffers");
    }
    return m_framebuffers[image_index];
}

std::optional<uint32_t> SwapChain::get_next_image_index(VkSemaphore &image_available) {
    uint32_t image_index;

    VkResult result =
        vkAcquireNextImageKHR(m_ctx->logical_device, m_swap_chain, UINT64_MAX,
                              image_available, VK_NULL_HANDLE, &image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return std::nullopt;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }
    return image_index;
}
