#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/DescriptorPool.h"
#include "render_engine/DescriptorSet.h"
#include "render_engine/Pipeline.h"
#include "render_engine/Sampler.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/Texture.h"
#include "render_engine/Window.h"
#include "render_engine/buffers/IndexBuffer.h"
#include "render_engine/buffers/UniformBuffer.h"
#include "render_engine/buffers/VertexBuffer.h"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class TextPipeline {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;

    size_t m_buffer_idx;
    std::vector<StorageBuffer> m_instance_buffers;
    VertexBuffer m_vertex_buffer;
    IndexBuffer m_index_buffer;

    VkDescriptorSetLayout m_descriptor_set_layout;
    DescriptorPool m_descriptor_pool;
    DescriptorSet m_descriptor_set;
    Pipeline m_pipeline;

    VkDescriptorSetLayout create_descriptor_set_layout();
    DescriptorSet create_descriptor_set(std::vector<UniformBuffer> &uniform_buffers,
                                        Sampler &sampler, Texture &texture);
    Pipeline create_pipeline(VkDescriptorSetLayout &descriptor_set_layout,
                             SwapChainManager &swap_chain_manager);
    std::vector<StorageBuffer> create_instance_buffers();

  public:
    TextPipeline(Window &window, std::shared_ptr<CoreGraphicsContext> ctx,
                 SwapChainManager &swap_chain,
                 std::vector<UniformBuffer> &uniform_buffers, Sampler &sampler,
                 Texture &texture);
    ~TextPipeline();

    void render_text(const VkCommandBuffer &command_buffer,
                     std::vector<StorageBufferObject> &&instance_data);
};
