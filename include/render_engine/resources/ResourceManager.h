#pragma once

#include "render_engine/resources/Resource.h"
#include "render_engine/resources/fonts/FontResource.h"
#include "render_engine/resources/images/ImageResource.h"
#include "render_engine/resources/shaders/ShaderResource.h"
#include <memory>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

template <typename R> struct is_valid_resource : std::false_type {};
template <> struct is_valid_resource<FontResource> : std::true_type {};
template <> struct is_valid_resource<ImageResource> : std::true_type {};
template <> struct is_valid_resource<ShaderResource> : std::true_type {};
template <typename T>
inline constexpr bool is_valid_resource_v = is_valid_resource<T>::value;

class ResourceManager {
  private:
    std::unordered_map<std::string, std::unique_ptr<Resource>> resources;
    ResourceManager() = default;

  public:
    static ResourceManager &get_instance() {
        static ResourceManager instance;
        return instance;
    }

    ResourceManager(const ResourceManager &other) = delete;
    ResourceManager &operator=(const ResourceManager &other) = delete;

    template <typename R, typename = std::enable_if_t<is_valid_resource_v<R>>>
    void register_resource(std::unique_ptr<R> resource) {
        resources[resource->name()] = std::move(resource);
    }

    template <typename R, typename = std::enable_if_t<is_valid_resource_v<R>>>
    const R *get_resource(const std::string &name) {
        auto it = resources.find(name);
        if (it == resources.end()) {
            std::stringstream ss;
            ss << "No resource " << name << " in ResourceManager";
            throw std::runtime_error(ss.str());
        }
        return dynamic_cast<const R *>(it->second.get());
    }
};

void register_all_fonts();
void register_all_images();
void register_all_shaders();
void register_shader(std::unique_ptr<ShaderResource> shader);
