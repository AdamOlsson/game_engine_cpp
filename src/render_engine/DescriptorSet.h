#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/Sampler.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/buffers/UniformBuffer.h"
#include <cstddef>
class DescriptorSet {
  private:
    std::shared_ptr<CoreGraphicsContext> ctx;
    size_t size;
    size_t next;
    std::vector<VkDescriptorSet> descriptor_sets;

  public:
    // TODO: Make this private, would require some proper handling of updating the
    // instance buffers through API calls. Propably need functions to get the new instance
    // buffer related to the current frame (get()) and current one (current()).
    std::vector<StorageBuffer> instance_buffers;

    DescriptorSet();
    DescriptorSet(std::shared_ptr<CoreGraphicsContext> ctx,
                  VkDescriptorSetLayout &descriptor_set_layout,
                  VkDescriptorPool &descriptor_pool, const int capacity,
                  std::vector<UniformBuffer> *uniform_buffers, Texture *texture,
                  Sampler *sampler);

    /* Example:
     *  DescriptorSet a;
     *  DescriptorSet b = std::move(a);
     */
    DescriptorSet(DescriptorSet &&other) noexcept; // Move constructor

    /* Example:
     *  DescriptorSet a, b;
     *  b = std::move(a);
     */
    DescriptorSet &operator=(DescriptorSet &&other) noexcept; // Move assignment

    /* Example:
     *  DescriptorSet a;
     *  DescriptorSet b = a;
     */
    DescriptorSet(const DescriptorSet &other) = delete; // Copy constructor

    /* Example:
     *  DescriptorSet a, b;
     *  b = a;
     */
    DescriptorSet &operator=(const DescriptorSet &other) = delete; // Copy assignement

    ~DescriptorSet();

    const VkDescriptorSet get();
};
