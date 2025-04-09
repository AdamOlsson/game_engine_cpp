#include "Texture.h"
#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/buffers/common.h"
#include "vulkan/vulkan_core.h"
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb/stb_image.h"

Texture::Texture()
    : ctx(nullptr), texture_image(VK_NULL_HANDLE), texture_image_memory(VK_NULL_HANDLE),
      texture_image_view(VK_NULL_HANDLE) {}

Texture::Texture(std::shared_ptr<CoreGraphicsContext> ctx,
                 const VkCommandPool &command_pool, const VkQueue &graphics_queue,
                 const char *filepath)
    : ctx(ctx) {

    // ------- Load image
    int texture_width;
    int texture_height;
    int texture_channels;

    // TODO: We probably need to include the bytes of the image into the build as with
    // shader code
    stbi_uc *pixels = stbi_load(filepath, &texture_width, &texture_height,
                                &texture_channels, STBI_rgb_alpha);
    VkDeviceSize texture_size = texture_width * texture_height * 4;

    if (!pixels) {
        std::stringstream ss;
        ss << "Failed to load image bytes from " << filepath << std::endl;
        throw std::runtime_error(ss.str());
    }

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    createBuffer(
        ctx->physicalDevice, ctx->device, texture_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        staging_buffer, staging_buffer_memory);

    void *data;
    vkMapMemory(ctx->device, staging_buffer_memory, 0, texture_size, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(texture_size));
    vkUnmapMemory(ctx->device, staging_buffer_memory);

    stbi_image_free(pixels);

    // ------- Create texture
    create_image(ctx.get(), texture_width, texture_height, texture_image,
                 texture_image_memory);

    transition_image_layout(ctx->device, command_pool, graphics_queue, texture_image,
                            VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copy_buffer_to_image(ctx->device, command_pool, graphics_queue, staging_buffer,
                         texture_image, static_cast<uint32_t>(texture_width),
                         static_cast<uint32_t>(texture_height));
    transition_image_layout(ctx->device, command_pool, graphics_queue, texture_image,
                            VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(ctx->device, staging_buffer, nullptr);
    vkFreeMemory(ctx->device, staging_buffer_memory, nullptr);

    // ------- Create texture view
    texture_image_view =
        create_image_view(ctx->device, texture_image, VK_FORMAT_R8G8B8A8_SRGB);
}

Texture::Texture(Texture &&other) noexcept
    : ctx(std::move(other.ctx)), texture_image(other.texture_image),
      texture_image_memory(other.texture_image_memory),
      texture_image_view(other.texture_image_view) {
    other.texture_image = VK_NULL_HANDLE;
    other.texture_image_memory = VK_NULL_HANDLE;
    other.texture_image_view = VK_NULL_HANDLE;
}

Texture &Texture::operator=(Texture &&other) noexcept {
    if (this != &other) {
        ctx = std::move(ctx);
        texture_image = other.texture_image;
        texture_image_memory = other.texture_image_memory;
        texture_image_view = other.texture_image_view;

        other.texture_image = VK_NULL_HANDLE;
        other.texture_image_memory = VK_NULL_HANDLE;
        other.texture_image_view = VK_NULL_HANDLE;
    }
    return *this;
}

Texture::~Texture() {
    vkDestroyImageView(ctx->device, texture_image_view, nullptr);
    vkDestroyImage(ctx->device, texture_image, nullptr);
    vkFreeMemory(ctx->device, texture_image_memory, nullptr);
}

VkImageView Texture::view() { return texture_image_view; }
