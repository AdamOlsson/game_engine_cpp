#pragma once

#include "render_engine/resources/Resource.h"
#include <cstdint>
#include <memory>
#include <vector>

class ShaderResource : public Resource {

  private:
    // All resources
    const std::string name_;
    const unsigned int length_;
    const uint8_t *bytes_;

    ShaderResource(const std::string &&name, const unsigned int length,
                   const uint8_t *bytes);

    friend class ShaderResourceBuilder;

  public:
    const uint8_t *bytes() const override;
    const std::string &name() const override;
    const unsigned int length() const override;
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

    ShaderResourceBuilder &name(const std::string &&name) {
        name_ = std::move(name);
        return *this;
    }

    ShaderResourceBuilder &length(const unsigned int length) {
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
