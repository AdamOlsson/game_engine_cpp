#include "Texture.h"
#include "render_engine/GeometryPipeline.h"
#include "render_engine/ImageData.h"
#include "render_engine/buffers/StagingBuffer.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>

Texture::Texture(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                 SwapChainManager &swap_chain_manager, const ImageData &image_data)
    : m_ctx(ctx), m_texture_image(TextureImage(
                      m_ctx, TextureImageDimension::from(image_data.dimension))) {

    StagingBuffer staging_buffer = StagingBuffer(m_ctx, image_data.size);

    auto device_queues = m_ctx->get_device_queues();
    m_texture_image.transition_image_layout(
        swap_chain_manager, device_queues.graphics_queue, VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    staging_buffer.transfer_image_to_device_image(
        image_data, m_texture_image, swap_chain_manager, device_queues.graphics_queue);

    m_texture_image.transition_image_layout(
        swap_chain_manager, device_queues.graphics_queue,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

Texture Texture::from_filepath(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                               SwapChainManager &swap_chain_manager,
                               const char *filepath) {
    const auto bytes = readFile(filepath);
    const ImageData image_data = ImageData::load_rgba_image(
        reinterpret_cast<const uint8_t *>(bytes.data()), bytes.size());
    return Texture(ctx, swap_chain_manager, image_data);
}

std::unique_ptr<Texture>
Texture::unique_from_filepath(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                              SwapChainManager &swap_chain_manager,
                              const char *filepath) {
    const auto bytes = readFile(filepath);
    const ImageData image_data = ImageData::load_rgba_image(
        reinterpret_cast<const uint8_t *>(bytes.data()), bytes.size());
    return std::move(std::make_unique<Texture>(ctx, swap_chain_manager, image_data));
}

Texture Texture::from_bytes(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                            SwapChainManager &swap_chain_manager, const uint8_t *bytes,
                            const unsigned int length) {
    const ImageData image_data = ImageData::load_rgba_image(bytes, length);
    return Texture(ctx, swap_chain_manager, image_data);
}

std::unique_ptr<Texture>
Texture::unique_from_bytes(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                           SwapChainManager &swap_chain_manager, const uint8_t *bytes,
                           const unsigned int length) {
    const ImageData image_data = ImageData::load_rgba_image(bytes, length);
    return std::move(std::make_unique<Texture>(ctx, swap_chain_manager, image_data));
}

Texture
Texture::from_image_resource(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                             SwapChainManager &swap_chain_manager,
                             const ImageResource *resource) {
    const ImageData image_data =
        ImageData::load_rgba_image(resource->bytes(), resource->length());
    return Texture(ctx, swap_chain_manager, image_data);
}

std::unique_ptr<Texture> Texture::unique_from_image_resource(
    std::shared_ptr<graphics_context::GraphicsContext> &ctx,
    SwapChainManager &swap_chain_manager, const ImageResource *resource) {
    const ImageData image_data =
        ImageData::load_rgba_image(resource->bytes(), resource->length());
    return std::move(std::make_unique<Texture>(ctx, swap_chain_manager, image_data));
}

Texture::Texture(Texture &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), m_texture_image(std::move(other.m_texture_image)) {}

Texture &Texture::operator=(Texture &&other) noexcept {
    if (this != &other) {
        m_ctx = std::move(m_ctx);
        m_texture_image = std::move(other.m_texture_image);
    }
    return *this;
}

Texture::~Texture() {}

VkImageView Texture::view() { return m_texture_image.m_image_view; }
