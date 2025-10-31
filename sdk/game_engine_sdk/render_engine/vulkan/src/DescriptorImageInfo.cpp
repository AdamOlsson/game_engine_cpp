#include "vulkan/DescriptorImageInfo.h"

vulkan::DescriptorImageInfo::DescriptorImageInfo(ImageView *image_view,
                                                 Sampler *image_sampler)
    : m_descriptor_image_info(VkDescriptorImageInfo{
          .sampler = *image_sampler,
          .imageView = *image_view,
          .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      }) {}
