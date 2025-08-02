#pragma once

#include "render_engine/TextureImage.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include "render_engine/resources/images/ImageResource.h"
#include "vulkan/vulkan_core.h"
#include <memory>

class Texture {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;
    TextureImage m_texture_image;

  public:
    Texture() = default;

    // As the make_unique needs to have access to the constructor it needs to be left
    // public
    Texture(std::shared_ptr<graphics_context::GraphicsContext> ctx,
            SwapChainManager &swap_chain_manager, const ImageData &image_data);

    static Texture from_filepath(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                                 SwapChainManager &swap_chain_manager,
                                 const char *filepath);
    static std::unique_ptr<Texture>
    unique_from_filepath(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                         SwapChainManager &swap_chain_manager, const char *filepath);

    static Texture from_bytes(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                              SwapChainManager &swap_chain_manager, const uint8_t *bytes,
                              const unsigned int length);

    static std::unique_ptr<Texture>
    unique_from_bytes(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                      SwapChainManager &swap_chain_manager, const uint8_t *bytes,
                      const unsigned int length);

    static Texture
    from_image_resource(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                        SwapChainManager &swap_chain_manager,
                        const ImageResource *resource);

    static std::unique_ptr<Texture>
    unique_from_image_resource(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                               SwapChainManager &swap_chain_manager,
                               const ImageResource *resource);

    static std::unique_ptr<Texture> unique_from_image_resource_name(
        std::shared_ptr<graphics_context::GraphicsContext> &ctx,
        SwapChainManager &swap_chain_manager, const std::string &resource_name);

    ~Texture() = default;

    Texture(Texture &&other) noexcept = default;
    Texture &operator=(Texture &&other) noexcept = default;

    Texture(const Texture &other) = delete;
    Texture &operator=(const Texture &other) = delete;

    VkImageView view();
};
