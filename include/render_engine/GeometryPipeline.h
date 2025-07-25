#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/DescriptorPool.h"
#include "render_engine/DescriptorSet.h"
#include "render_engine/Pipeline.h"
#include "render_engine/Sampler.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/Texture.h"
#include "render_engine/Window.h"
#include "render_engine/buffers/GpuBuffer.h"
#include "render_engine/buffers/IndexBuffer.h"
#include "render_engine/buffers/VertexBuffer.h"
#include "shape.h"
#include "vulkan/vulkan_core.h"
#include <iostream>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const int MAX_FRAMES_IN_FLIGHT = 2;

VKAPI_ATTR inline VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

    std::cout << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

std::vector<char> readFile(const std::string filename);
VkDescriptorPool createDescriptorPool(VkDevice &device, const int capacity);

struct GeometryInstanceBufferObject {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec4 color;
    alignas(4) glm::float32_t rotation;
    alignas(4) glm::uint32 shape_type;
    alignas(16) Shape shape;
    alignas(16) glm::vec4 uvwt;

    GeometryInstanceBufferObject(glm::vec3 position, glm::vec4 color,
                                 glm::float32_t rotation, Shape shape, glm::vec4 uvwt)
        : position(position), color(color), rotation(rotation),
          shape_type(shape.encode_shape_type()), shape(shape), uvwt(uvwt) {}

    std::string to_string() const {
        return std::format("GeometryInstanceBufferObject {{\n"
                           "  position:   ({:.3f}, {:.3f}, {:.3f})\n"
                           "  color:      ({:.3f}, {:.3f}, {:.3f}, {:.3f})\n"
                           "  rotation:   {:.3f}°\n"
                           "  shape_type: {}\n"
                           "  shape:      {}\n"
                           "  uvwt:       ({:.3f}, {:.3f}, {:.3f}, {:.3f})\n"
                           "}}",
                           position.x, position.y, position.z, color.r, color.g, color.b,
                           color.a, rotation, shape_type,
                           shape.to_string(), // Assuming Shape has a to_string() method
                           uvwt.x, uvwt.y, uvwt.z, uvwt.w);
    }

    friend std::ostream &operator<<(std::ostream &os,
                                    const GeometryInstanceBufferObject &obj) {
        return os << obj.to_string();
    }
};

class GeometryPipeline {
  private:
    const uint32_t m_num_storage_buffers = MAX_FRAMES_IN_FLIGHT * 4;
    const uint32_t m_num_uniform_buffers = MAX_FRAMES_IN_FLIGHT * 4;
    const uint32_t m_num_samplers = MAX_FRAMES_IN_FLIGHT * 4;
    const uint32_t m_descriptor_pool_capacity = MAX_FRAMES_IN_FLIGHT * 4;

    std::shared_ptr<CoreGraphicsContext> m_ctx;

    VkDescriptorSetLayout m_descriptor_set_layout;
    Pipeline m_pipeline;
    DescriptorPool m_descriptor_pool;

    SwapGpuBuffer<GeometryInstanceBufferObject> m_circle_instance_buffers;
    DescriptorSet m_circle_descriptor_set;
    VertexBuffer m_circle_vertex_buffer;
    IndexBuffer m_circle_index_buffer;

    SwapGpuBuffer<GeometryInstanceBufferObject> m_triangle_instance_buffers;
    DescriptorSet m_triangle_descriptor_set;
    VertexBuffer m_triangle_vertex_buffer;
    IndexBuffer m_triangle_index_buffer;

    SwapGpuBuffer<GeometryInstanceBufferObject> m_rectangle_instance_buffers;
    DescriptorSet m_rectangle_descriptor_set;
    VertexBuffer m_rectangle_vertex_buffer;
    IndexBuffer m_rectangle_index_buffer;

    SwapGpuBuffer<GeometryInstanceBufferObject> m_hexagon_instance_buffers;
    DescriptorSet m_hexagon_descriptor_set;
    VertexBuffer m_hexagon_vertex_buffer;
    IndexBuffer m_hexagon_index_buffer;

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    bool framebufferResized = false;

    VkDescriptorSetLayout create_descriptor_set_layout();

    Pipeline create_pipeline(VkDescriptorSetLayout &descriptorSetLayout,
                             SwapChainManager &swap_chain_manager);

    void record_draw_command(const VkCommandBuffer &command_buffer,
                             DescriptorSet &descriptor_set,
                             const ShapeTypeEncoding shape_type_encoding,
                             const VertexBuffer &vertex_buffer,
                             const IndexBuffer &index_buffer, const size_t num_instances);

  public:
    GeometryPipeline(Window &window, std::shared_ptr<CoreGraphicsContext> ctx,
                     SwapChainManager &swap_chain_manager,
                     SwapUniformBuffer<WindowDimension<float>> &uniform_buffers,
                     Sampler &sampler, Texture &texture);

    ~GeometryPipeline();

    GpuBuffer<GeometryInstanceBufferObject> &get_circle_instance_buffer();
    GpuBuffer<GeometryInstanceBufferObject> &get_triangle_instance_buffer();
    GpuBuffer<GeometryInstanceBufferObject> &get_rectangle_instance_buffer();
    GpuBuffer<GeometryInstanceBufferObject> &get_hexagon_instance_buffer();

    // TODO: These render function should merge into one generic call
    void render_circles(const VkCommandBuffer &command_buffer);
    void render_triangles(const VkCommandBuffer &command_buffer);
    void render_rectangles(const VkCommandBuffer &command_buffer);
    void render_hexagons(const VkCommandBuffer &command_buffer);
};
