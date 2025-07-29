#pragma once

#include "render_engine/Instance.h"
#include "render_engine/Surface.h"

class PhysicalDevice {
  private:
    bool is_device_suitable(const VkPhysicalDevice &physical_device,
                            const Surface &surface);
    bool check_device_extension_support(const VkPhysicalDevice &physical_device);

    VkPhysicalDevice pick_physical_device(const Instance &instance,
                                          const Surface &surface);

  public:
    VkPhysicalDevice physical_device;

    PhysicalDevice(const Instance &instance, const Surface &surface);
    ~PhysicalDevice() = default;

    PhysicalDevice(PhysicalDevice &&) noexcept = default;
    PhysicalDevice &operator=(PhysicalDevice &&) noexcept = default;
    PhysicalDevice(const PhysicalDevice &) = delete;
    PhysicalDevice &operator=(const PhysicalDevice &) = delete;

    operator VkPhysicalDevice() const { return physical_device; }
};
