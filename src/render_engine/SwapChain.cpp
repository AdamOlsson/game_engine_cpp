#include "SwapChain.h"
#include "render_engine/buffers/common.h"
#include "util.h"
#include "vulkan/vulkan_core.h"
#include <optional>

std::vector<VkImageView> createImageViews(VkDevice &device,
                                          std::vector<VkImage> &swapChainImages,
                                          VkFormat &swapChainImageFormat);

VkSurfaceFormatKHR
chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

VkPresentModeKHR
chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

VkExtent2D chooseSwapExtent(GLFWwindow &window,
                            const VkSurfaceCapabilitiesKHR &capabilities);

SwapChain::SwapChain(std::shared_ptr<CoreGraphicsContext> ctx, const Window &window)

    : ctx(ctx) {
    SwapChainSupportDetails swap_chain_support =
        querySwapChainSupport(ctx->physicalDevice, ctx->surface);

    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swap_chain_support.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swap_chain_support.presentModes);
    VkExtent2D extent = chooseSwapExtent(*window.window, swap_chain_support.capabilities);

    uint32_t imageCount = swap_chain_support.capabilities.minImageCount + 1;
    if (swap_chain_support.capabilities.maxImageCount > 0 &&
        imageCount > swap_chain_support.capabilities.maxImageCount) {
        imageCount = swap_chain_support.capabilities.maxImageCount;
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

    createInfo.preTransform = swap_chain_support.capabilities.currentTransform;
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

    if (vkCreateSwapchainKHR(ctx->device, &createInfo, nullptr, &swap_chain) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(ctx->device, swap_chain, &imageCount, nullptr);

    images.resize(imageCount);
    vkGetSwapchainImagesKHR(ctx->device, swap_chain, &imageCount, images.data());

    image_format = surfaceFormat.format;
    this->extent = extent;

    image_views = createImageViews(ctx->device, images, image_format);
}

SwapChain::SwapChain(SwapChain &&other) noexcept
    : ctx(std::move(other.ctx)), swap_chain(other.swap_chain),
      images(std::move(other.images)), image_format(other.image_format),
      extent(other.extent), image_views(std::move(other.image_views)) {}

SwapChain &SwapChain::operator=(SwapChain &&other) noexcept {
    if (this != &other) {
        ctx = std::move(other.ctx);
        swap_chain = other.swap_chain;
        images = std::move(other.images);
        image_format = other.image_format;
        extent = other.extent;
        image_views = other.image_views;

        other.ctx = nullptr;
        other.cleanup_done = false;
        other.swap_chain = VK_NULL_HANDLE;
        other.images = {};
        other.image_views = {};
    }
    return *this;
}

SwapChain::~SwapChain() {
    for (size_t i = 0; i < image_views.size(); i++) {
        vkDestroyImageView(ctx->device, image_views[i], nullptr);
    }
    vkDestroySwapchainKHR(ctx->device, swap_chain, nullptr);
}

std::vector<VkImageView> createImageViews(VkDevice &device,
                                          std::vector<VkImage> &swapChainImages,
                                          VkFormat &swapChainImageFormat) {
    std::vector<VkImageView> swapChainImageViews;
    swapChainImageViews.resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews[i] =
            create_image_view(device, swapChainImages[i], swapChainImageFormat);
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
