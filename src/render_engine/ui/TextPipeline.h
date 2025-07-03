#pragma once

#include "ElementProperties.h"
#include "glm/fwd.hpp"
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

namespace ui {

struct TextSegmentBufferObject {
    alignas(16) glm::vec3 color = colors::WHITE;
    alignas(4) glm::float32_t rotation = 0.0f;
    alignas(4) glm::uint32_t font_size = 128;

    std::string to_string() const {
        return std::format("TextSegmentBufferObject {{\n"
                           "  color:     ({:.3f}, {:.3f}, {:.3f})\n"
                           "  rotation:  {:.3f}\n"
                           "  font_size: {:d}\n"
                           "}}",
                           color.x, color.y, color.z, rotation, font_size);
    }

    friend std::ostream &operator<<(std::ostream &os,
                                    const TextSegmentBufferObject &obj) {
        return os << obj.to_string();
    }
};

struct CharacterInstanceBufferObject {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec4 uvwt;

    std::string to_string() const {
        return std::format("CharacterInstanceBufferObject {{\n"
                           "  position:   ({:.3f}, {:.3f}, {:.3f})\n"
                           "  uvwt:       ({:.3f}, {:.3f}, {:.3f}, {:.3f})\n"
                           "}}",
                           position.x, position.y, position.z, uvwt.x, uvwt.y, uvwt.z,
                           uvwt.w);
    }

    friend std::ostream &operator<<(std::ostream &os,
                                    const CharacterInstanceBufferObject &obj) {
        return os << obj.to_string();
    }
};

class TextPipeline {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;

    SwapStorageBuffer<CharacterInstanceBufferObject> m_character_buffers;
    SwapStorageBuffer<TextSegmentBufferObject> m_text_segment_buffers;
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

  public:
    TextPipeline(Window &window, std::shared_ptr<CoreGraphicsContext> ctx,
                 SwapChainManager &swap_chain,
                 std::vector<UniformBuffer> &uniform_buffers, Sampler &sampler,
                 Texture &texture);
    ~TextPipeline();

    StorageBuffer<CharacterInstanceBufferObject> &get_instance_buffer();

    void render_text(const VkCommandBuffer &command_buffer,
                     ui::ElementProperties::FontProperties &text_props);
};
} // namespace ui
