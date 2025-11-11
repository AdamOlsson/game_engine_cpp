#pragma once
#include <cstdint>
#include <string>
namespace graphics_pipeline {
// Temporary until the resource manager is removed
class Resource {
  public:
    virtual const uint8_t *bytes() const = 0;
    virtual const std::string &name() const = 0;
    virtual const unsigned int length() const = 0;
    virtual ~Resource() = default;
};
} // namespace graphics_pipeline
