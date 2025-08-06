#pragma once

#include "Instance.h"
#include "Surface.h"
#include "logger.h"

namespace vulkan {
namespace device {

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    uint32_t get_image_count() const {
        uint32_t image_count = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
            image_count = capabilities.maxImageCount;
        }
        return image_count;
    }

    VkSurfaceFormatKHR
    choose_swap_surface_format(const VkSurfaceFormatKHR &&requested_format) const {
        for (const auto &available_format : formats) {
            if (available_format.format == requested_format.format &&
                available_format.colorSpace == requested_format.colorSpace) {
                return available_format;
            }
        }
        logger::warning("Requested format not found, "
                        "returning first available");
        return formats[0];
    }

    VkPresentModeKHR
    choose_swap_present_mode(const VkPresentModeKHR &&requested_present_mode) const {
        for (const auto &available_present_mode : presentModes) {
            if (available_present_mode == requested_present_mode) {
                return available_present_mode;
            }
        }
        logger::warning("Requested present mode not found, "
                        "returning VK_PRESENT_MODE_FIFO_KHR");

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D choose_swap_extent(
        const window::WindowDimension<uint32_t> &window_framebuffer) const {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {

            VkExtent2D window_extent = {window_framebuffer.width,
                                        window_framebuffer.height};

            window_extent.width =
                std::clamp(window_extent.width, capabilities.minImageExtent.width,
                           capabilities.maxImageExtent.width);
            window_extent.height =
                std::clamp(window_extent.height, capabilities.minImageExtent.height,
                           capabilities.maxImageExtent.height);

            return window_extent;
        }
    }
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

class PhysicalDevice {
  private:
    VkPhysicalDevice m_physical_device;

    bool is_device_suitable(const VkPhysicalDevice &physical_device,
                            const Surface &surface) const;
    bool check_device_extension_support(const VkPhysicalDevice &physical_device) const;

    VkPhysicalDevice pick_physical_device(const Instance &instance,
                                          const Surface &surface) const;

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

    SwapChainSupportDetails query_swap_chain_support(const Surface &surface) const;
    static SwapChainSupportDetails
    query_swap_chain_support(const VkPhysicalDevice &physical_device,
                             const Surface &surface);

    uint32_t find_memory_type(const uint32_t type_filter,
                              const VkMemoryPropertyFlags properties) const;
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
    void wait_for_fence(const VkFence &fence) const;

    void reset_fence(const VkFence &fence) const;
};

} // namespace device
} // namespace vulkan
