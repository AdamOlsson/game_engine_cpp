#pragma once

#include <cstdint>
#include <string>
class Resource {
  public:
    virtual const uint8_t *bytes() const = 0;
    virtual const std::string &name() const = 0;
    virtual const unsigned int length() const = 0;
    virtual ~Resource() = default;
};
