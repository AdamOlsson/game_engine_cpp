#include "vulkan/ImageView.h"
#include "vulkan/vulkan_core.h"

vulkan::ImageView::ImageView() : m_image_view(VK_NULL_HANDLE) {}

vulkan::ImageView::ImageView(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                             const VkImage &image, const VkFormat format)
    : m_ctx(ctx), m_image_view(create_image_view(image, format)) {}

vulkan::ImageView::~ImageView() {
    if (m_image_view == VK_NULL_HANDLE) {
        return;
    }
    vkDestroyImageView(m_ctx->logical_device, m_image_view, nullptr);
}

vulkan::ImageView::ImageView(ImageView &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), m_image_view(other.m_image_view) {
    other.m_image_view = VK_NULL_HANDLE;
}

vulkan::ImageView &vulkan::ImageView::operator=(ImageView &&other) noexcept {
    if (this != &other) {
        if (m_image_view != VK_NULL_HANDLE) {
            vkDestroyImageView(m_ctx->logical_device, m_image_view, nullptr);
        }
        m_ctx = std::move(other.m_ctx);
        m_image_view = other.m_image_view;

        other.m_image_view = VK_NULL_HANDLE;
    }
    return *this;
}

VkImageView vulkan::ImageView::create_image_view(const VkImage &image,
                                                 const VkFormat &format) {
    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = image;
    create_info.format = format;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;
    VkImageView image_view;
    if (vkCreateImageView(m_ctx->logical_device, &create_info, nullptr, &image_view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image views!");
    }
    return image_view;
}
