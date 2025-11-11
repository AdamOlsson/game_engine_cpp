#pragma once

#include "Resource.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace graphics_pipeline {

class ShaderResource : public Resource {
  private:
    std::string name_;
    unsigned int length_;
    uint8_t *bytes_;

    ShaderResource(std::string &&name, unsigned int length, uint8_t *bytes);

    friend class ShaderResourceBuilder;

  public:
    const uint8_t *bytes() const;
    const std::string &name() const;
    const unsigned int length() const;

    ShaderResource(ShaderResource &&other) noexcept = default;
    ShaderResource &operator=(ShaderResource &&other) noexcept = default;
    ShaderResource(const ShaderResource &other) = delete;
    ShaderResource &operator=(const ShaderResource &other) = delete;
};

std::vector<std::unique_ptr<ShaderResource>> fetch_all_shaders();

class ShaderResourceBuilder {
  private:
    // All resources
    std::string name_;
    unsigned int length_ = 0;
    unsigned char *bytes_ = nullptr;

  public:
    ShaderResourceBuilder() = default;
    ~ShaderResourceBuilder() = default;
    ShaderResourceBuilder(const ShaderResourceBuilder &) = delete;
    ShaderResourceBuilder &operator=(const ShaderResourceBuilder &) = delete;

    ShaderResourceBuilder &name(std::string &&name) {
        name_ = std::move(name);
        return *this;
    }

    ShaderResourceBuilder &length(unsigned int length) {
        length_ = length;
        return *this;
    }

    ShaderResourceBuilder &bytes(unsigned char *bytes) {
        bytes_ = bytes;
        return *this;
    }

    std::unique_ptr<ShaderResource> build() {
        // TODO: enforce required params
        return std::unique_ptr<ShaderResource>(new ShaderResource(
            std::move(name_), length_, static_cast<uint8_t *>(bytes_)));
    }
};

} // namespace graphics_pipeline
