#pragma once

#include "glm/fwd.hpp"
#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/DescriptorPool.h"
#include "render_engine/DescriptorSet.h"
#include "render_engine/Pipeline.h"
#include "render_engine/Sampler.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/IndexBuffer.h"
#include "render_engine/buffers/VertexBuffer.h"
#include "render_engine/colors.h"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <vulkan/vulkan.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ui {

struct TextSegmentBufferObject {
    alignas(16) glm::vec4 font_color = colors::WHITE;
    alignas(4) uint32_t font_size = 128;
    alignas(4) float font_rotation = 0.0f;
    alignas(4) float font_weight = 0.4f;
    alignas(4) float font_sharpness = 2.5f;
};

struct CharacterInstanceBufferObject {
    alignas(8) glm::vec2 position;
    alignas(16) glm::vec4 uvwt;
    alignas(4) uint32_t text_segment_idx = 0;
};

static_assert(sizeof(TextSegmentBufferObject) % 16 == 0,
              "Struct must be 16-byte aligned");
static_assert(sizeof(CharacterInstanceBufferObject) % 16 == 0,
              "Struct must be 16-byte aligned");

std::ostream &operator<<(std::ostream &os, const TextSegmentBufferObject &obj);
std::ostream &operator<<(std::ostream &os, const CharacterInstanceBufferObject &obj);

class TextPipeline {
  private:
    // 2 x num resources per frame
    const uint32_t m_num_storage_buffers = 2 * 2;
    const uint32_t m_num_uniform_buffers = 2 * 1;
    const uint32_t m_num_samplers = 2 * 1;
    const uint32_t m_descriptor_pool_capacity = 2;

    std::shared_ptr<CoreGraphicsContext> m_ctx;

    SwapGpuBuffer<CharacterInstanceBufferObject> m_character_buffers;
    SwapGpuBuffer<TextSegmentBufferObject> m_text_segment_buffers;
    VertexBuffer m_vertex_buffer;
    IndexBuffer m_index_buffer;

    VkDescriptorSetLayout m_descriptor_set_layout;
    DescriptorPool m_descriptor_pool;
    DescriptorSet m_descriptor_set;
    Pipeline m_pipeline;

    VkDescriptorSetLayout create_descriptor_set_layout();
    DescriptorSet create_descriptor_set(
        SwapUniformBuffer<window::WindowDimension<float>> &uniform_buffers,
        Sampler &sampler, Texture &texture);
    Pipeline create_pipeline(VkDescriptorSetLayout &descriptor_set_layout,
                             SwapChainManager &swap_chain_manager);

  public:
    TextPipeline(std::shared_ptr<CoreGraphicsContext> ctx, SwapChainManager &swap_chain,
                 SwapUniformBuffer<window::WindowDimension<float>> &uniform_buffers,
                 Sampler &sampler, Texture &texture);
    ~TextPipeline();

    GpuBuffer<CharacterInstanceBufferObject> &get_character_buffer();
    GpuBuffer<TextSegmentBufferObject> &get_text_segment_buffer();

    void render_text(const VkCommandBuffer &command_buffer);
};
} // namespace ui
