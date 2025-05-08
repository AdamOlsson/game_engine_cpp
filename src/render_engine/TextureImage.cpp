#include "TextureImage.h"
#include "buffers/common.h"
#include "render_engine/SingleTimeCommandBuffer.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/buffers/common.h"

TextureImage::TextureImage()
    : m_ctx(nullptr), m_image(nullptr), m_image_memory(nullptr), m_image_view(nullptr),
      m_dimension(TextureImageDimension{0, 0}) {}

TextureImage::TextureImage(std::shared_ptr<CoreGraphicsContext> ctx,
                           const TextureImageDimension &dim)
    : m_ctx(ctx), m_dimension(dim) {

    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = dim.width;
    image_info.extent.height = dim.height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = m_format;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.flags = 0;

    if (vkCreateImage(ctx->device, &image_info, nullptr, &m_image) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture image");
    }

    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(ctx->device, m_image, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = find_memory_type(
        ctx.get(), mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (vkAllocateMemory(ctx->device, &alloc_info, nullptr, &m_image_memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture image memory");
    }

    vkBindImageMemory(ctx->device, m_image, m_image_memory, 0);
    m_image_view = create_image_view(ctx.get(), m_image, VK_FORMAT_R8G8B8A8_SRGB);
}

TextureImage::TextureImage(TextureImage &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), m_image(other.m_image),
      m_image_memory(other.m_image_memory), m_image_view(other.m_image_view),
      m_dimension(other.m_dimension) {
    other.m_image = nullptr;
    other.m_image_memory = nullptr;
    other.m_image_view = nullptr;
    other.m_dimension = TextureImageDimension{0, 0};
}

TextureImage &TextureImage::operator=(TextureImage &&other) noexcept {
    if (this != &other) {
        m_ctx = std::move(other.m_ctx);
        m_image = other.m_image;
        m_image_memory = other.m_image_memory;
        m_image_view = other.m_image_view;
        m_dimension = other.m_dimension;

        other.m_image = nullptr;
        other.m_image_memory = nullptr;
        other.m_image_view = nullptr;
        other.m_dimension = TextureImageDimension{0, 0};
    }

    return *this;
}

TextureImage::~TextureImage() {
    vkDestroyImageView(m_ctx->device, m_image_view, nullptr);
    vkDestroyImage(m_ctx->device, m_image, nullptr);
    vkFreeMemory(m_ctx->device, m_image_memory, nullptr);
}

void TextureImage::transition_image_layout(SwapChainManager &swap_chain_manager,
                                           const VkQueue &graphics_queue,
                                           const VkImageLayout old_layout,
                                           const VkImageLayout new_layout) {
    SingleTimeCommandBuffer command_buffer =
        swap_chain_manager.get_single_time_command_buffer();
    command_buffer.begin();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;

    VkPipelineStageFlagBits2 source_stage;
    VkPipelineStageFlagBits2 destination_stage;
    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
        new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("Unsupported layout transition!");
    }

    vkCmdPipelineBarrier(command_buffer.m_command_buffer, source_stage, destination_stage,
                         0, 0, nullptr, 0, nullptr, 1, &barrier);

    command_buffer.end();
    command_buffer.submit(graphics_queue);
}
