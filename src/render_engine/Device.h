#pragma once

#include "render_engine/Instance.h"
#include "render_engine/Surface.h"

namespace device {
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

class PhysicalDevice {
  private:
    VkPhysicalDevice m_physical_device;

    bool is_device_suitable(const VkPhysicalDevice &physical_device,
                            const Surface &surface);
    bool check_device_extension_support(const VkPhysicalDevice &physical_device);

    VkPhysicalDevice pick_physical_device(const Instance &instance,
                                          const Surface &surface);

  public:
    PhysicalDevice(const Instance &instance, const Surface &surface);
    ~PhysicalDevice() = default;

    PhysicalDevice(PhysicalDevice &&) noexcept = default;
    PhysicalDevice &operator=(PhysicalDevice &&) noexcept = default;
    PhysicalDevice(const PhysicalDevice &) = delete;
    PhysicalDevice &operator=(const PhysicalDevice &) = delete;

    operator VkPhysicalDevice() const { return m_physical_device; }

    QueueFamilyIndices find_queue_families(const Surface &surface) const;
    static QueueFamilyIndices find_queue_families(const VkPhysicalDevice &physical_device,
                                                  const Surface &surface);
};

class LogicalDevice {
  private:
    bool m_enable_validation_layers;
    VkDevice m_logical_device;
    VkDevice create_logical_device(const Surface &surface,
                                   const PhysicalDevice &physical_device,
                                   const std::vector<const char *> &device_extensions);

  public:
    LogicalDevice(const bool enable_validation_layers, const Surface &surface,
                  const PhysicalDevice &physical_device);
    ~LogicalDevice();

    LogicalDevice(LogicalDevice &&) noexcept = default;
    LogicalDevice &operator=(LogicalDevice &&) noexcept = default;
    LogicalDevice(const LogicalDevice &) = delete;
    LogicalDevice &operator=(const LogicalDevice &) = delete;

    operator VkDevice() const { return m_logical_device; }

    void wait_idle();
};

} // namespace device
