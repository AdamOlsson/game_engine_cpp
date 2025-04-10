#include "ResourceManager.h"
#include "render_engine/resources/fonts/FontResource.h"
#include "render_engine/resources/images/ImageResource.h"

void register_all_fonts() {
    ResourceManager &instance = ResourceManager::get_instance();

    auto fonts = fetch_all_fonts();
    for (auto &f : fonts) {
        instance.register_resource<FontResource>(std::move(f));
    }
}

void register_all_images() {
    ResourceManager &instance = ResourceManager::get_instance();

    auto fonts = fetch_all_images();
    for (auto &f : fonts) {
        instance.register_resource<ImageResource>(std::move(f));
    }
}

void register_all_shaders() {
    ResourceManager &instance = ResourceManager::get_instance();

    auto fonts = fetch_all_shaders();
    for (auto &f : fonts) {
        instance.register_resource<ShaderResource>(std::move(f));
    }
}
