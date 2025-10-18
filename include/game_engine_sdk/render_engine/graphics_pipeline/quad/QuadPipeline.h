#pragma once
#include "game_engine_sdk/render_engine/buffers/IndexBuffer.h"
#include "game_engine_sdk/render_engine/buffers/VertexBuffer.h"
#include "game_engine_sdk/render_engine/descriptors/SwapDescriptorSet.h"
#include "game_engine_sdk/render_engine/graphics_context/GraphicsContext.h"
#include "game_engine_sdk/render_engine/vulkan/CommandBuffer.h"
#include "game_engine_sdk/render_engine/vulkan/DescriptorBufferInfo.h"
#include "game_engine_sdk/render_engine/vulkan/DescriptorImageInfo.h"
#include "game_engine_sdk/render_engine/vulkan/DescriptorPool.h"
#include <memory>

namespace graphics_pipeline {

struct QuadPipelineUBO {

    std::string to_string() const {
        return std::format("QuadPipelineUBO {{\n"
                           "}}");
    }

    friend std::ostream &operator<<(std::ostream &os, const QuadPipelineUBO &obj) {
        return os << obj.to_string();
    }
};

struct QuadPipelineDescriptorSetOpts {
    unsigned int num_sets = 2;
    std::vector<vulkan::DescriptorBufferInfo> storage_buffer_refs = {};
    std::vector<vulkan::DescriptorBufferInfo> uniform_buffer_refs = {};
    std::vector<vulkan::DescriptorImageInfo> combined_image_sampler_infos = {};
};

class QuadPipelineDescriptorSet {
  private:
    SwapDescriptorSet m_descriptor_set;

  public:
    QuadPipelineDescriptorSet(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                              vulkan::DescriptorPool &descriptor_pool,
                              QuadPipelineDescriptorSetOpts &&opts);

    const vulkan::DescriptorSetLayout &get_layout();
    const SwapDescriptorSet *handle() const;
};

class QuadPipeline {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;

    VertexBuffer m_quad_vertex_buffer;
    IndexBuffer m_quad_index_buffer;

    VkPipelineLayout m_pipeline_layout;
    VkPipeline m_pipeline;

  public:
    QuadPipeline(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                 CommandBufferManager *command_buffer_manager);

    void render(const vulkan::CommandBuffer &command_buffer,
                const QuadPipelineDescriptorSet &descriptor_set);
};

} // namespace graphics_pipeline
