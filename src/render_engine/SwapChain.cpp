#include "SwapChain.h"
#include "render_engine/buffers/common.h"
#include "render_engine/util.h"

SwapChain::SwapChain(std::shared_ptr<CoreGraphicsContext> ctx, const Window &window)
    : m_ctx(ctx), m_next_frame_buffer(0) {

    SwapChainSupportDetails swap_chain_support =
        querySwapChainSupport(m_ctx->physicalDevice, m_ctx->surface);

    uint32_t image_count = get_image_count(swap_chain_support);

    VkSurfaceFormatKHR surface_format =
        choose_swap_surface_format(swap_chain_support.formats);
    m_extent = choose_swap_extent(*window.window, swap_chain_support.capabilities);

    m_swap_chain = create_swap_chain(image_count, surface_format, swap_chain_support);

    m_images = create_swap_chain_images(image_count);
    m_image_views = create_image_views(surface_format.format);
    m_render_pass = create_render_pass(surface_format.format);
    m_frame_buffers = create_frame_buffers();
}

SwapChain::~SwapChain() {
    vkDestroyRenderPass(m_ctx->device, m_render_pass, nullptr);
    for (size_t i = 0; i < m_image_views.size(); i++) {
        vkDestroyImageView(m_ctx->device, m_image_views[i], nullptr);
    }
    vkDestroySwapchainKHR(m_ctx->device, m_swap_chain, nullptr);
    for (size_t i = 0; i < m_frame_buffers.size(); i++) {
        vkDestroyFramebuffer(m_ctx->device, m_frame_buffers[i], nullptr);
    }
}

uint32_t SwapChain::get_image_count(SwapChainSupportDetails &swap_chain_support) {
    uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
    if (swap_chain_support.capabilities.maxImageCount > 0 &&
        image_count > swap_chain_support.capabilities.maxImageCount) {
        image_count = swap_chain_support.capabilities.maxImageCount;
    }
    return image_count;
}

VkSurfaceFormatKHR SwapChain::choose_swap_surface_format(
    const std::vector<VkSurfaceFormatKHR> &available_formats) {
    for (const auto &availableFormat : available_formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return available_formats[0];
}

VkPresentModeKHR SwapChain::choose_swap_present_mode(
    const std::vector<VkPresentModeKHR> &available_present_modes) {
    for (const auto &availablePresentMode : available_present_modes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::choose_swap_extent(GLFWwindow &window,
                                         const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(&window, &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                                   static_cast<uint32_t>(height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                       capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                       capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

VkSwapchainKHR SwapChain::create_swap_chain(uint32_t image_count,
                                            VkSurfaceFormatKHR &surface_format,
                                            SwapChainSupportDetails &swap_chain_support) {
    VkPresentModeKHR present_mode =
        choose_swap_present_mode(swap_chain_support.presentModes);

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_ctx->surface;
    createInfo.minImageCount = image_count;
    createInfo.imageFormat = surface_format.format;
    createInfo.imageColorSpace = surface_format.colorSpace;
    createInfo.imageExtent = m_extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(m_ctx->physicalDevice, m_ctx->surface);
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
    if (vkCreateSwapchainKHR(m_ctx->device, &createInfo, nullptr, &swap_chain) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }
    return swap_chain;
}

std::vector<VkImage> SwapChain::create_swap_chain_images(uint32_t image_count) {
    std::vector<VkImage> images;
    images.resize(image_count);
    vkGetSwapchainImagesKHR(m_ctx->device, m_swap_chain, &image_count, images.data());
    return images;
}

std::vector<VkImageView> SwapChain::create_image_views(VkFormat &image_format) {
    std::vector<VkImageView> swapChainImageViews;
    swapChainImageViews.resize(m_images.size());
    for (size_t i = 0; i < m_images.size(); i++) {
        swapChainImageViews[i] =
            create_image_view(m_ctx->device, m_images[i], image_format);
    }
    return swapChainImageViews;
}

std::vector<VkFramebuffer> SwapChain::create_frame_buffers() {

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

        if (vkCreateFramebuffer(m_ctx->device, &framebufferInfo, nullptr,
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
    if (vkCreateRenderPass(m_ctx->device, &render_pass_info, nullptr, &render_pass) !=
        VK_SUCCESS) {
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
        vkAcquireNextImageKHR(m_ctx->device, m_swap_chain, UINT64_MAX, image_available,
                              VK_NULL_HANDLE, &image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return std::nullopt;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }
    return image_index;
}
