#include "graphics_pipeline/SwapDescriptorSet.h"
#include "vulkan/DescriptorBufferInfo.h"
#include "vulkan/DescriptorImageInfo.h"
#include "vulkan/DescriptorPool.h"
#include <vector>
#pragma

namespace graphics_pipeline::geometry {

struct GeometryPipelineDescriptorSetOpts {
    unsigned int num_sets = 2;
    std::vector<vulkan::DescriptorBufferInfo> storage_buffer_refs = {};
    std::vector<vulkan::DescriptorBufferInfo> uniform_buffer_refs = {};
    std::vector<vulkan::DescriptorImageInfo> combined_image_sampler_infos = {};
};

class GeometryPipelineDescriptorSet {
  private:
    SwapDescriptorSet m_descriptor_set;

    SwapDescriptorSet
    build_descriptor_set(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                         vulkan::DescriptorPool &descriptor_pool,
                         GeometryPipelineDescriptorSetOpts &&opts);

  public:
    GeometryPipelineDescriptorSet(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                                  vulkan::DescriptorPool &descriptor_pool,
                                  GeometryPipelineDescriptorSetOpts &&opts);

    vulkan::DescriptorSetLayout &get_layout();
    SwapDescriptorSet *handle();
    vulkan::DescriptorSet get_next();
    vulkan::DescriptorSet get_current();
    void rotate();
};

}; // namespace graphics_pipeline::geometry
