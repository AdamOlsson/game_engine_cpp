#pragma once

#include "game_engine_sdk/render_engine/CommandBufferManager.h"
#include "game_engine_sdk/render_engine/resources/images/ImageResource.h"
#include "game_engine_sdk/render_engine/vulkan/TextureImage.h"
#include "game_engine_sdk/render_engine/vulkan/context/GraphicsContext.h"
#include "image/Image.h"
#include <memory>

class Texture {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;
    vulkan::TextureImage m_texture_image;
    vulkan::ImageView m_texture_image_view;

  public:
    Texture() = default;

    // As the make_unique needs to have access to the constructor it needs to be left
    // public
    Texture(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
            CommandBufferManager *command_buffer_manager, const image::Image &image_data);

    static Texture from_filepath(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                                 CommandBufferManager *command_buffer_manager,
                                 const char *filepath);
    static std::unique_ptr<Texture>
    unique_from_filepath(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                         CommandBufferManager *command_buffer_manager,
                         const char *filepath);

    static Texture from_bytes(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                              CommandBufferManager *command_buffer_manager,
                              const uint8_t *bytes, const unsigned int length);

    static std::unique_ptr<Texture>
    unique_from_bytes(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                      CommandBufferManager *command_buffer_manager, const uint8_t *bytes,
                      const unsigned int length);

    static Texture
    from_image_resource(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                        CommandBufferManager *command_buffer_manager,
                        const ImageResource *resource);

    static std::unique_ptr<Texture>
    unique_from_image_resource(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                               CommandBufferManager *command_buffer_manager,
                               const ImageResource *resource);

    static std::unique_ptr<Texture> unique_from_image_resource_name(
        std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
        CommandBufferManager *command_buffer_manager, const std::string &resource_name);

    static std::unique_ptr<Texture>
    unique_empty(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                 CommandBufferManager *command_buffer_manager);

    static Texture empty(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                         CommandBufferManager *command_buffer_manager);

    ~Texture() = default;

    Texture(Texture &&other) noexcept = default;
    Texture &operator=(Texture &&other) noexcept = default;

    Texture(const Texture &other) = delete;
    Texture &operator=(const Texture &other) = delete;

    vulkan::ImageView *view();

    vulkan::TextureImageDimension dimension() const;
};
