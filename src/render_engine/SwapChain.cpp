#include "SwapChain.h"
#include "io.h"
#include "util.h"
#include <optional>

std::vector<VkImageView> createImageViews(VkDevice &device,
                                          std::vector<VkImage> &swapChainImages,
                                          VkFormat &swapChainImageFormat) {
    std::vector<VkImageView> swapChainImageViews;
    swapChainImageViews.resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
    return swapChainImageViews;
}

VkSurfaceFormatKHR
chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR
chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(GLFWwindow &window,
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

/*SwapChain::SwapChain(VkPhysicalDevice &physicalDevice, VkDevice &device,*/
/*                     VkSurfaceKHR &surface, GLFWwindow &window)*/
SwapChain::SwapChain(std::shared_ptr<CoreGraphicsContext> ctx, GLFWwindow &window)

    : ctx(ctx), cleanup_done(false) {
    SwapChainSupportDetails swapChainSupport =
        querySwapChainSupport(ctx->physicalDevice, ctx->surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(window, swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = ctx->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    // Note from the tutorial:
    //  It is also possible that you'll render images to a separate image
    //  first to perform operations like post-processing. In that case you may
    //  use a value like VK_IMAGE_USAGE_TRANSFER_DST_BIT instead and use a
    //  memory operation to transfer the rendered image to a swap chain image.
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(ctx->physicalDevice, ctx->surface);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                     indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        // Note: Can also be done in exclusive but requires passing ownership of
        // frame buffers which is not covered by the tutorial so far.
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    // Note from the tutorial:
    // With Vulkan it's possible that your swap chain becomes invalid or
    // unoptimized while your application is running, for example because the
    // window was resized. In that case the swap chain actually needs to be
    // recreated from scratch and a reference to the old one must be specified
    // in this field. This is a complex topic that we'll learn more about in a
    // future chapter.
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(ctx->device, &createInfo, nullptr, &swapChain) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(ctx->device, swapChain, &imageCount, nullptr);

    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(ctx->device, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;

    swapChainImageViews =
        createImageViews(ctx->device, swapChainImages, swapChainImageFormat);
}

SwapChain::~SwapChain() { cleanup(); }

void SwapChain::cleanup() {

    if (cleanup_done) {
        return;
    }

    /*if (device == nullptr) {*/
    /*    std::cout << "Can't cleanup swap chain because device is null." << std::endl;*/
    /*    return;*/
    /*}*/

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        vkDestroyImageView(ctx->device, swapChainImageViews[i], nullptr);
    }
    vkDestroySwapchainKHR(ctx->device, swapChain, nullptr);
    cleanup_done = true;
}

std::vector<VkFramebuffer> SwapChain::createFramebuffers(VkRenderPass &renderPass) {

    std::vector<VkFramebuffer> swapChainFramebuffers;
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        VkImageView attachments[] = {swapChainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(ctx->device, &framebufferInfo, nullptr,
                                &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
    return swapChainFramebuffers;
}
