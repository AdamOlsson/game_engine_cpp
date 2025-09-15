#include "render_engine/resources/images/ImageResource.h"
#include "render_engine/resources/images/dog/dog.h"
#include <memory>

ImageResource::ImageResource(const std::string &&name, const unsigned int length,
                             const uint8_t *bytes, const uint32_t width_px,
                             const uint32_t height_px, const uint8_t num_channels)
    : name_(std::move(name)), length_(length), bytes_(bytes), width_px(width_px),
      height_px(height_px), num_channels(num_channels) {}

const uint8_t *ImageResource::bytes() const { return bytes_; }

const std::string &ImageResource::name() const { return name_; }

const unsigned int ImageResource::length() const { return length_; }

std::vector<std::unique_ptr<ImageResource>> fetch_all_images() {
    std::vector<std ::unique_ptr<ImageResource>> images{};
    images.push_back(DogImage::create_resource());
    return images;
}
