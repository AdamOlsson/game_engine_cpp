#include "DescriptorImageInfo.h"

vulkan::DescriptorImageInfo::DescriptorImageInfo(ImageView *image_view,
                                                 Sampler *image_sampler)
    : m_descriptor_image_info(VkDescriptorImageInfo{
          .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
          .imageView = *image_view,
          .sampler = *image_sampler,
      }) {}
