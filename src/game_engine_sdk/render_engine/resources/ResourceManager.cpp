#include "game_engine_sdk/render_engine/resources/ResourceManager.h"
#include "game_engine_sdk/render_engine/resources/fonts/FontResource.h"
#include "game_engine_sdk/render_engine/resources/images/ImageResource.h"

void register_all_fonts() {
    ResourceManager &instance = ResourceManager::get_instance();

    auto fonts = fetch_all_fonts();
    for (auto &f : fonts) {
        instance.register_resource<FontResource>(std::move(f));
    }
}

void register_all_images() {
    ResourceManager &instance = ResourceManager::get_instance();

    auto images = fetch_all_images();
    for (auto &i : images) {
        instance.register_resource<ImageResource>(std::move(i));
    }
}

void register_all_shaders() {
    ResourceManager &instance = ResourceManager::get_instance();

    auto shaders = fetch_all_shaders();
    for (auto &s : shaders) {
        instance.register_resource<ShaderResource>(std::move(s));
    }
}

void register_shader(std::unique_ptr<ShaderResource> shader) {
    ResourceManager &instance = ResourceManager::get_instance();
    instance.register_resource<ShaderResource>(std::move(shader));
}
