#include "SwapChain.h"
#include "render_engine/buffers/common.h"
#include "render_engine/util.h"
#include "vulkan/vulkan_core.h"
#include <optional>

SwapChain::SwapChain(std::shared_ptr<CoreGraphicsContext> ctx, const Window &window)

    : m_window(&window), m_ctx(ctx), m_next_frame_buffer(0) {
    SwapChainSupportDetails swap_chain_support =
        querySwapChainSupport(m_ctx->physicalDevice, m_ctx->surface);

    VkSurfaceFormatKHR surfaceFormat =
        choose_swap_surface_format(swap_chain_support.formats);
    VkPresentModeKHR presentMode =
        choose_swap_present_mode(swap_chain_support.presentModes);
    m_extent = choose_swap_extent(*m_window->window, swap_chain_support.capabilities);

    uint32_t imageCount = swap_chain_support.capabilities.minImageCount + 1;
    if (swap_chain_support.capabilities.maxImageCount > 0 &&
        imageCount > swap_chain_support.capabilities.maxImageCount) {
        imageCount = swap_chain_support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_ctx->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
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
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_ctx->device, &createInfo, nullptr, &m_swap_chain) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    m_images = create_swap_chain_images(imageCount);
    m_image_format = surfaceFormat.format;

    image_views = create_image_views();
    m_render_pass = create_render_pass();
    m_frame_buffer = create_frame_buffers();
}

std::vector<VkImage> SwapChain::create_swap_chain_images(uint32_t image_count) {
    std::vector<VkImage> images;
    images.resize(image_count);
    vkGetSwapchainImagesKHR(m_ctx->device, m_swap_chain, &image_count, images.data());
    return images;
}
SwapChain::SwapChain(SwapChain &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), m_swap_chain(other.m_swap_chain),
      m_images(std::move(other.m_images)), m_image_format(other.m_image_format),
      m_extent(other.m_extent), image_views(std::move(other.image_views)) {}

SwapChain &SwapChain::operator=(SwapChain &&other) noexcept {
    if (this != &other) {
        m_ctx = std::move(other.m_ctx);
        m_swap_chain = other.m_swap_chain;
        m_images = std::move(other.m_images);
        m_image_format = other.m_image_format;
        m_extent = other.m_extent;
        image_views = other.image_views;

        other.m_ctx = nullptr;
        other.m_swap_chain = VK_NULL_HANDLE;
        other.m_images = {};
        other.image_views = {};
    }
    return *this;
}

SwapChain::~SwapChain() {
    vkDestroyRenderPass(m_ctx->device, m_render_pass, nullptr);
    // ######## This needs to be recreated when recreating a new swap chain
    for (size_t i = 0; i < image_views.size(); i++) {
        vkDestroyImageView(m_ctx->device, image_views[i], nullptr);
    }
    vkDestroySwapchainKHR(m_ctx->device, m_swap_chain, nullptr);
    for (size_t i = 0; i < m_frame_buffer.size(); i++) {
        vkDestroyFramebuffer(m_ctx->device, m_frame_buffer[i], nullptr);
    }
    // ########
}

std::vector<VkImageView> SwapChain::create_image_views() {
    std::vector<VkImageView> swapChainImageViews;
    swapChainImageViews.resize(m_images.size());
    for (size_t i = 0; i < m_images.size(); i++) {
        swapChainImageViews[i] =
            create_image_view(m_ctx->device, m_images[i], m_image_format);
    }
    return swapChainImageViews;
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

std::vector<VkFramebuffer> SwapChain::create_frame_buffers() {

    const size_t capacity = image_views.size();
    std::vector<VkFramebuffer> swapChainFramebuffers;
    swapChainFramebuffers.resize(capacity);

    for (size_t i = 0; i < capacity; i++) {
        VkImageView attachments[] = {image_views[i]};

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

VkRenderPass SwapChain::create_render_pass() {
    VkAttachmentDescription color_attachment{};
    color_attachment.format = m_image_format;
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
    auto &buf = m_frame_buffer[m_next_frame_buffer];
    m_next_frame_buffer = ++m_next_frame_buffer % m_frame_buffer.size();
    return buf;
}

/*std::optional<int> SwapChain::begin_render_pass(VkCommandBuffer &command_buffer) {}*/
