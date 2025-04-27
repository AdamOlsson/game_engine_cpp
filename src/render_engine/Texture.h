#pragma once

#include "render_engine/CommandHandler.h"
#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/TextureImage.h"
#include "render_engine/resources/images/ImageResource.h"
#include "vulkan/vulkan_core.h"
#include <memory>

class Texture {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;
    TextureImage m_texture_image;

  public:
    Texture() = default;

    // As the make_unique needs to have access to the constructor it needs to be left
    // public
    Texture(std::shared_ptr<CoreGraphicsContext> ctx, const CommandPool &command_pool,
            const VkQueue &graphics_queue, const ImageData &image_data);

    static Texture from_filepath(std::shared_ptr<CoreGraphicsContext> &ctx,
                                 const CommandPool &command_pool,
                                 const VkQueue &graphics_queue, const char *filepath);
    static std::unique_ptr<Texture>
    unique_from_filepath(std::shared_ptr<CoreGraphicsContext> &ctx,
                         const CommandPool &command_pool, const VkQueue &graphics_queue,
                         const char *filepath);

    static Texture from_bytes(std::shared_ptr<CoreGraphicsContext> &ctx,
                              const CommandPool &command_pool,
                              const VkQueue &graphics_queue, const uint8_t *bytes,
                              const unsigned int length);

    static std::unique_ptr<Texture>
    unique_from_bytes(std::shared_ptr<CoreGraphicsContext> &ctx,
                      const CommandPool &command_pool, const VkQueue &graphics_queue,
                      const uint8_t *bytes, const unsigned int length);

    static Texture from_image_resource(std::shared_ptr<CoreGraphicsContext> &ctx,
                                       const CommandPool &command_pool,
                                       const VkQueue &graphics_queue,
                                       const ImageResource *resource);

    static std::unique_ptr<Texture> unique_from_image_resource(
        std::shared_ptr<CoreGraphicsContext> &ctx, const CommandPool &command_pool,
        const VkQueue &graphics_queue, const ImageResource *resource);

    ~Texture();

    Texture(Texture &&other) noexcept;
    Texture &operator=(Texture &&other) noexcept;

    Texture(const Texture &other) = delete;
    Texture &operator=(const Texture &other) = delete;

    VkImageView view();
};
