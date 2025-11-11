#pragma once

#include "game_engine_sdk/render_engine/Texture.h"
#include "game_engine_sdk/render_engine/descriptors/SwapDescriptorSet.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/GraphicsPipeline.h"
#include "util/colors.h"
#include "vulkan/DescriptorImageInfo.h"
#include "vulkan/DescriptorPool.h"
#include "vulkan/Sampler.h"
#include "vulkan/SwapChainManager.h"
#include "vulkan/Vertex.h"
#include "vulkan/buffers/GpuBuffer.h"
#include "vulkan/buffers/IndexBuffer.h"
#include "vulkan/buffers/VertexBuffer.h"
#include "vulkan/context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <iostream>
#include <memory>
#include <vulkan/vulkan.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace graphics_pipeline {
const int MAX_FRAMES_IN_FLIGHT = 2;

VKAPI_ATTR inline VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

    std::cout << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

struct GeometryInstanceBufferObject {
    alignas(16) glm::vec3 center = glm::vec3(0.0f);
    alignas(8) glm::vec2 dimension = glm::vec2(0.0f);
    alignas(4) glm::float32_t rotation = 0.0f;
    alignas(4) glm::uint32_t texture_idx = 0;
    alignas(16) glm::vec4 color = util::colors::TRANSPARENT;
    alignas(16) glm::vec4 uvwt = glm::vec4(-1.0f);
    struct BorderProperties {
        alignas(16) glm::vec4 color = util::colors::TRANSPARENT;
        alignas(4) glm::float32_t thickness = 0.0f;
        alignas(4) glm::float32_t radius = 0.0f;
    } border;

    std::string to_string() const {
        return std::format("GeometryInstanceBufferObject {{\n"
                           "  center:     ({:.3f}, {:.3f}, {:.3f})\n"
                           "  dimension:  ({:.3f}, {:.3f})\n"
                           "  rotation:   {:.3f}°\n"
                           "  color:      ({:.3f}, {:.3f}, {:.3f}, {:.3f})\n"
                           "  uvwt:       ({:.3f}, {:.3f}, {:.3f}, {:.3f})\n"
                           "}}",
                           center.x, center.y, center.z, dimension.x, dimension.y,
                           rotation, color.r, color.g, color.b, color.a, uvwt.x, uvwt.y,
                           uvwt.z, uvwt.w);
    }

    friend std::ostream &operator<<(std::ostream &os,
                                    const GeometryInstanceBufferObject &obj) {
        return os << obj.to_string();
    }
};

struct GeometryPipelineOptions {
    std::vector<vulkan::DescriptorImageInfo> combined_image_samplers;
};

class GeometryPipeline {
  private:
    const uint32_t m_num_storage_buffers = MAX_FRAMES_IN_FLIGHT * 4;
    const uint32_t m_num_uniform_buffers = MAX_FRAMES_IN_FLIGHT * 4 * 2;
    const uint32_t m_num_combined_image_samplers = MAX_FRAMES_IN_FLIGHT * 4 * 5;
    const uint32_t m_descriptor_pool_capacity = MAX_FRAMES_IN_FLIGHT * 4;

    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    GeometryPipelineOptions m_opts;
    std::optional<Texture> m_empty_texture;
    std::optional<vulkan::Sampler> m_empty_sampler;

    vulkan::DescriptorPool m_descriptor_pool;
    vulkan::buffers::VertexBuffer m_quad_vertex_buffer;
    vulkan::buffers::IndexBuffer m_quad_index_buffer;

    vulkan::buffers::SwapStorageBuffer<GeometryInstanceBufferObject>
        m_circle_instance_buffers;
    vulkan::buffers::SwapUniformBuffer<VertexUBO> m_circle_vertices_ubo;
    SwapDescriptorSet m_circle_descriptor_set;

    vulkan::buffers::SwapStorageBuffer<GeometryInstanceBufferObject>
        m_triangle_instance_buffers;
    vulkan::buffers::SwapUniformBuffer<VertexUBO> m_triangle_vertices_ubo;
    SwapDescriptorSet m_triangle_descriptor_set;

    vulkan::buffers::SwapStorageBuffer<GeometryInstanceBufferObject>
        m_rectangle_instance_buffers;
    vulkan::buffers::SwapUniformBuffer<VertexUBO> m_rectangle_vertices_ubo;
    SwapDescriptorSet m_rectangle_descriptor_set;

    vulkan::buffers::SwapStorageBuffer<GeometryInstanceBufferObject>
        m_hexagon_instance_buffers;
    vulkan::buffers::SwapUniformBuffer<VertexUBO> m_hexagon_vertices_ubo;
    SwapDescriptorSet m_hexagon_descriptor_set;

    graphics_pipeline::GraphicsPipeline m_graphics_pipeline;

    /*void record_draw_command(const VkCommandBuffer &command_buffer,*/
    /*                         DescriptorSet &descriptor_set,*/
    /*                         const ShapeTypeEncoding shape_type_encoding,*/
    /*                         const VertexBuffer &vertex_buffer,*/
    /*                         const IndexBuffer &index_buffer, const size_t
     * num_instances);*/

  public:
    GeometryPipeline(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                     vulkan::CommandBufferManager *command_buffer_manager,
                     vulkan::SwapChainManager &swap_chain_manager,
                     std::optional<GeometryPipelineOptions> opts);

    ~GeometryPipeline();

    vulkan::buffers::StorageBuffer<GeometryInstanceBufferObject> &
    get_circle_instance_buffer();
    vulkan::buffers::StorageBuffer<GeometryInstanceBufferObject> &
    get_triangle_instance_buffer();
    vulkan::buffers::StorageBuffer<GeometryInstanceBufferObject> &
    get_rectangle_instance_buffer();
    vulkan::buffers::StorageBuffer<GeometryInstanceBufferObject> &
    get_hexagon_instance_buffer();

    void render_circles(const VkCommandBuffer &command_buffer, glm::mat4 &camera_matrix);
    void render_triangles(const VkCommandBuffer &command_buffer,
                          glm::mat4 &camera_matrix);
    void render_rectangles(const VkCommandBuffer &command_buffer,
                           glm::mat4 &camera_matrix);
    void render_hexagons(const VkCommandBuffer &command_buffer, glm::mat4 &camera_matrix);
};

} // namespace graphics_pipeline
