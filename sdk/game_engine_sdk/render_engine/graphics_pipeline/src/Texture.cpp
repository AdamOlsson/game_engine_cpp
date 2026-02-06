#include "graphics_pipeline/Texture.h"
#include "util/file.h"
#include "vulkan/buffers/StagingBuffer.h"
#include "vulkan/vulkan_core.h"
#include <memory>

graphics_pipeline::Texture::Texture(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                                    vulkan::CommandBufferManager *command_buffer_manager,
                                    const image::Image &image_data)
    : m_ctx(ctx), m_texture_image(vulkan::TextureImage(
                      m_ctx, vulkan::TextureImageDimension::from(image_data.dimensions))),
      m_texture_image_view(m_texture_image.create_view()) {

    vulkan::buffers::StagingBuffer staging_buffer =
        vulkan::buffers::StagingBuffer(m_ctx, image_data.size);

    m_texture_image.transition_image_layout(command_buffer_manager,
                                            VK_IMAGE_LAYOUT_UNDEFINED,
                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    staging_buffer.transfer_image_to_device_image(command_buffer_manager, image_data,
                                                  m_texture_image);

    m_texture_image.transition_image_layout(command_buffer_manager,
                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

graphics_pipeline::Texture graphics_pipeline::Texture::from_filepath(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::CommandBufferManager *command_buffer_manager, const char *filepath) {
    const auto bytes = util::file::read_file(filepath);
    const auto image_data = image::Image::load_rgba_image(
        reinterpret_cast<const uint8_t *>(bytes.data()), bytes.size());
    return Texture(ctx, command_buffer_manager, image_data);
}

std::unique_ptr<graphics_pipeline::Texture>
graphics_pipeline::Texture::unique_from_filepath(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::CommandBufferManager *command_buffer_manager, const char *filepath) {
    const auto bytes = util::file::read_file(filepath);
    const auto image_data = image::Image::load_rgba_image(
        reinterpret_cast<const uint8_t *>(bytes.data()), bytes.size());
    return std::move(std::make_unique<Texture>(ctx, command_buffer_manager, image_data));
}

graphics_pipeline::Texture graphics_pipeline::Texture::from_bytes(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::CommandBufferManager *command_buffer_manager, const uint8_t *bytes,
    const unsigned int length) {
    const auto image_data = image::Image::load_rgba_image(bytes, length);
    return Texture(ctx, command_buffer_manager, image_data);
}

std::unique_ptr<graphics_pipeline::Texture> graphics_pipeline::Texture::unique_from_bytes(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::CommandBufferManager *command_buffer_manager, const uint8_t *bytes,
    const unsigned int length) {
    const auto image_data = image::Image::load_rgba_image(bytes, length);
    return std::make_unique<Texture>(ctx, command_buffer_manager, image_data);
}

graphics_pipeline::Texture graphics_pipeline::Texture::from_image_resource(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::CommandBufferManager *command_buffer_manager, const ImageResource *resource) {
    const auto image_data =
        image::Image::load_rgba_image(resource->bytes(), resource->length());
    return Texture(ctx, command_buffer_manager, image_data);
}

std::unique_ptr<graphics_pipeline::Texture>
graphics_pipeline::Texture::unique_from_image_resource(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::CommandBufferManager *command_buffer_manager,
    const graphics_pipeline::ImageResource *resource) {
    const auto image_data =
        image::Image::load_rgba_image(resource->bytes(), resource->length());
    return std::move(std::make_unique<Texture>(ctx, command_buffer_manager, image_data));
}

std::unique_ptr<graphics_pipeline::Texture> graphics_pipeline::Texture::unique_empty(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::CommandBufferManager *command_buffer_manager) {
    const auto image_data = image::Image::empty();
    return std::move(std::make_unique<Texture>(ctx, command_buffer_manager, image_data));
}

graphics_pipeline::Texture
graphics_pipeline::Texture::empty(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                                  vulkan::CommandBufferManager *command_buffer_manager) {
    const auto image_data = image::Image::empty();
    return std::move(Texture(ctx, command_buffer_manager, image_data));
}

vulkan::ImageView *graphics_pipeline::Texture::view() { return &m_texture_image_view; }

vulkan::TextureImageDimension graphics_pipeline::Texture::dimension() const {
    return m_texture_image.m_dimension;
}
