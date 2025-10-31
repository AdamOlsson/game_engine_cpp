#include "game_engine_sdk/render_engine/Texture.h"
#include "game_engine_sdk/render_engine/buffers/StagingBuffer.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/GraphicsPipeline.h"
#include "game_engine_sdk/render_engine/resources/ResourceManager.h"
#include "vulkan/vulkan_core.h"
#include <memory>

Texture::Texture(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                 CommandBufferManager *command_buffer_manager,
                 const image::Image &image_data)
    : m_ctx(ctx), m_texture_image(vulkan::TextureImage(
                      m_ctx, vulkan::TextureImageDimension::from(image_data.dimensions))),
      m_texture_image_view(m_texture_image.create_view()) {

    StagingBuffer staging_buffer = StagingBuffer(m_ctx, image_data.size);

    m_texture_image.transition_image_layout(command_buffer_manager,
                                            VK_IMAGE_LAYOUT_UNDEFINED,
                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    staging_buffer.transfer_image_to_device_image(command_buffer_manager, image_data,
                                                  m_texture_image);

    m_texture_image.transition_image_layout(command_buffer_manager,
                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

Texture Texture::from_filepath(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                               CommandBufferManager *command_buffer_manager,
                               const char *filepath) {
    const auto bytes = graphics_pipeline::readFile(filepath);
    const auto image_data = image::Image::load_rgba_image(
        reinterpret_cast<const uint8_t *>(bytes.data()), bytes.size());
    return Texture(ctx, command_buffer_manager, image_data);
}

std::unique_ptr<Texture>
Texture::unique_from_filepath(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                              CommandBufferManager *command_buffer_manager,
                              const char *filepath) {
    const auto bytes = graphics_pipeline::readFile(filepath);
    const auto image_data = image::Image::load_rgba_image(
        reinterpret_cast<const uint8_t *>(bytes.data()), bytes.size());
    return std::move(std::make_unique<Texture>(ctx, command_buffer_manager, image_data));
}

Texture Texture::from_bytes(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                            CommandBufferManager *command_buffer_manager,
                            const uint8_t *bytes, const unsigned int length) {
    const auto image_data = image::Image::load_rgba_image(bytes, length);
    return Texture(ctx, command_buffer_manager, image_data);
}

std::unique_ptr<Texture>
Texture::unique_from_bytes(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                           CommandBufferManager *command_buffer_manager,
                           const uint8_t *bytes, const unsigned int length) {
    const auto image_data = image::Image::load_rgba_image(bytes, length);
    return std::move(std::make_unique<Texture>(ctx, command_buffer_manager, image_data));
}

Texture
Texture::from_image_resource(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                             CommandBufferManager *command_buffer_manager,
                             const ImageResource *resource) {
    const auto image_data =
        image::Image::load_rgba_image(resource->bytes(), resource->length());
    return Texture(ctx, command_buffer_manager, image_data);
}

std::unique_ptr<Texture> Texture::unique_from_image_resource(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    CommandBufferManager *command_buffer_manager, const ImageResource *resource) {
    const auto image_data =
        image::Image::load_rgba_image(resource->bytes(), resource->length());
    return std::move(std::make_unique<Texture>(ctx, command_buffer_manager, image_data));
}

std::unique_ptr<Texture> Texture::unique_from_image_resource_name(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    CommandBufferManager *command_buffer_manager, const std::string &resource_name) {
    auto resource =
        ResourceManager::get_instance().get_resource<ImageResource>(resource_name);
    const auto image_data =
        image::Image::load_rgba_image(resource->bytes(), resource->length());
    return std::move(std::make_unique<Texture>(ctx, command_buffer_manager, image_data));
}

std::unique_ptr<Texture>
Texture::unique_empty(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                      CommandBufferManager *command_buffer_manager) {
    const auto image_data = image::Image::empty();
    return std::move(std::make_unique<Texture>(ctx, command_buffer_manager, image_data));
}

Texture Texture::empty(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                       CommandBufferManager *command_buffer_manager) {
    const auto image_data = image::Image::empty();
    return std::move(Texture(ctx, command_buffer_manager, image_data));
}

vulkan::ImageView *Texture::view() { return &m_texture_image_view; }

vulkan::TextureImageDimension Texture::dimension() const {
    return m_texture_image.m_dimension;
}
