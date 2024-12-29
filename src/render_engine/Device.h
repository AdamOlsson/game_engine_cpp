#pragma once
#include <vulkan/vulkan.h>

struct Device {
    VkDevice device;
    Device(VkDevice device) : device(device) {}
    ~Device() { vkDestroyDevice(device, nullptr); }
};
