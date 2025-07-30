#include "SwapChain.h"
#include "render_engine/buffers/common.h"
#include "vulkan/vulkan_core.h"

SwapChain::SwapChain(std::shared_ptr<graphics_context::GraphicsContext> ctx)
    : m_ctx(ctx), m_next_frame_buffer(0) {

    graphics_context::device::SwapChainSupportDetails swap_chain_support =
        m_ctx->physical_device.query_swap_chain_support(m_ctx->surface);

    uint32_t image_count = swap_chain_support.get_image_count();

    VkSurfaceFormatKHR surface_format = swap_chain_support.choose_swap_surface_format(
        {.format = VK_FORMAT_B8G8R8A8_SRGB,
         .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR});

    auto window_framebuffer_size = m_ctx->window->get_framebuffer_size<uint32_t>();
    m_extent = swap_chain_support.choose_swap_extent(window_framebuffer_size);

    m_swap_chain = create_swap_chain(image_count, surface_format, swap_chain_support);

    m_images = create_swap_chain_images(image_count);
    m_image_views = create_image_views(surface_format.format);
    m_render_pass = create_render_pass(surface_format.format);
    m_frame_buffers = create_framebuffers();
}

SwapChain::~SwapChain() {
    vkDestroyRenderPass(m_ctx->logical_device, m_render_pass, nullptr);
    for (size_t i = 0; i < m_image_views.size(); i++) {
        vkDestroyImageView(m_ctx->logical_device, m_image_views[i], nullptr);
    }
    vkDestroySwapchainKHR(m_ctx->logical_device, m_swap_chain, nullptr);
    for (size_t i = 0; i < m_frame_buffers.size(); i++) {
        vkDestroyFramebuffer(m_ctx->logical_device, m_frame_buffers[i], nullptr);
    }
}

VkSwapchainKHR SwapChain::create_swap_chain(
    uint32_t image_count, VkSurfaceFormatKHR &surface_format,
    graphics_context::device::SwapChainSupportDetails &swap_chain_support) {
    VkPresentModeKHR present_mode =
        swap_chain_support.choose_swap_present_mode(VK_PRESENT_MODE_MAILBOX_KHR);

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_ctx->surface.surface;
    createInfo.minImageCount = image_count;
    createInfo.imageFormat = surface_format.format;
    createInfo.imageColorSpace = surface_format.colorSpace;
    createInfo.imageExtent = m_extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    graphics_context::device::QueueFamilyIndices indices =
        m_ctx->physical_device.find_queue_families(m_ctx->surface);

    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                     indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swap_chain_support.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = present_mode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    VkSwapchainKHR swap_chain;
    if (vkCreateSwapchainKHR(m_ctx->logical_device, &createInfo, nullptr, &swap_chain) !=
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

std::vector<VkImageView> SwapChain::create_image_views(VkFormat &image_format) {
    std::vector<VkImageView> swapChainImageViews;
    swapChainImageViews.resize(m_images.size());
    for (size_t i = 0; i < m_images.size(); i++) {
        swapChainImageViews[i] =
            create_image_view(m_ctx.get(), m_images[i], image_format);
    }
    return swapChainImageViews;
}

std::vector<VkFramebuffer> SwapChain::create_framebuffers() {

    const size_t capacity = m_image_views.size();
    std::vector<VkFramebuffer> swapChainFramebuffers;
    swapChainFramebuffers.resize(capacity);

    for (size_t i = 0; i < capacity; i++) {
        VkImageView attachments[] = {m_image_views[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_render_pass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_extent.width;
        framebufferInfo.height = m_extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_ctx->logical_device, &framebufferInfo, nullptr,
                                &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
    return swapChainFramebuffers;
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

VkFramebuffer SwapChain::get_frame_buffer() {
    auto &buf = m_frame_buffers[m_next_frame_buffer];
    m_next_frame_buffer = ++m_next_frame_buffer % m_frame_buffers.size();
    return buf;
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
