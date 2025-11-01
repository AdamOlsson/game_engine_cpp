#pragma once

#include "game_engine_sdk/render_engine/colors.h"
#include "game_engine_sdk/render_engine/descriptors/SwapDescriptorSet.h"
#include "game_engine_sdk/render_engine/fonts/Font.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/GraphicsPipeline.h"
#include "game_engine_sdk/render_engine/ui/ElementProperties.h"
#include "glm/fwd.hpp"
#include "vulkan/DescriptorImageInfo.h"
#include "vulkan/DescriptorPool.h"
#include "vulkan/SwapChainManager.h"
#include "vulkan/buffers/GpuBuffer.h"
#include "vulkan/buffers/IndexBuffer.h"
#include "vulkan/buffers/VertexBuffer.h"
#include "vulkan/context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <vulkan/vulkan.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace graphics_pipeline {

struct TextSegmentBufferObject {
    alignas(16) glm::vec4 font_color = colors::WHITE;
    alignas(4) uint32_t font_size = 128;
    alignas(4) float font_rotation = 0.0f;
    alignas(4) float font_weight = 0.5f;
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

struct TextPipelineOptions {
    std::vector<vulkan::DescriptorImageInfo> combined_image_samplers;
};

class TextPipeline {
  private:
    // 2 x num resources per frame
    const uint32_t m_num_storage_buffers = 2 * 2;
    const uint32_t m_num_uniform_buffers = 2 * 1;
    const uint32_t m_num_samplers = 2 * 1;
    const uint32_t m_descriptor_pool_capacity = 2;

    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    std::unique_ptr<Font> m_font;
    TextPipelineOptions m_opts;

    vulkan::buffers::SwapStorageBuffer<CharacterInstanceBufferObject> m_character_buffers;
    vulkan::buffers::SwapStorageBuffer<TextSegmentBufferObject> m_text_segment_buffers;
    vulkan::buffers::VertexBuffer m_vertex_buffer;
    vulkan::buffers::IndexBuffer m_index_buffer;

    vulkan::DescriptorPool m_descriptor_pool;
    SwapDescriptorSet m_descriptor_set;
    graphics_pipeline::GraphicsPipeline m_graphics_pipeline;

  public:
    TextPipeline(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                 vulkan::CommandBufferManager *command_buffer_manager,
                 vulkan::SwapChainManager &swap_chain_manager,
                 std::unique_ptr<Font> font);
    ~TextPipeline();

    vulkan::buffers::StorageBuffer<CharacterInstanceBufferObject> &get_character_buffer();
    vulkan::buffers::StorageBuffer<TextSegmentBufferObject> &get_text_segment_buffer();

    void render_text(const VkCommandBuffer &command_buffer);

    void text_kerning(const std::string_view text,
                      const ui::ElementProperties properties);
};
} // namespace graphics_pipeline
