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
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/buffers/UniformBuffer.h"
#include "render_engine/buffers/VertexBuffer.h"
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

class GeometryPipeline {
  private:
    const uint32_t m_num_storage_buffers = MAX_FRAMES_IN_FLIGHT * 4;
    const uint32_t m_num_uniform_buffers = MAX_FRAMES_IN_FLIGHT * 4;
    const uint32_t m_num_samplers = MAX_FRAMES_IN_FLIGHT * 4;
    const uint32_t m_descriptor_pool_capacity = MAX_FRAMES_IN_FLIGHT * 4;

    std::shared_ptr<CoreGraphicsContext> m_ctx;

    Pipeline m_pipeline;
    DescriptorPool m_descriptor_pool;

    SwapStorageBuffer<StorageBufferObject> m_circle_instance_buffers;
    DescriptorSet m_circle_descriptor_set;
    VertexBuffer m_circle_vertex_buffer;
    IndexBuffer m_circle_index_buffer;

    SwapStorageBuffer<StorageBufferObject> m_triangle_instance_buffers;
    DescriptorSet m_triangle_descriptor_set;
    VertexBuffer m_triangle_vertex_buffer;
    IndexBuffer m_triangle_index_buffer;

    SwapStorageBuffer<StorageBufferObject> m_rectangle_instance_buffers;
    DescriptorSet m_rectangle_descriptor_set;
    VertexBuffer m_rectangle_vertex_buffer;
    IndexBuffer m_rectangle_index_buffer;

    SwapStorageBuffer<StorageBufferObject> m_hexagon_instance_buffers;
    DescriptorSet m_hexagon_descriptor_set;
    VertexBuffer m_hexagon_vertex_buffer;
    IndexBuffer m_hexagon_index_buffer;

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    bool framebufferResized = false;

    Pipeline create_pipeline(VkDescriptorSetLayout &descriptorSetLayout,
                             SwapChainManager &swap_chain_manager);
    bool checkDeviceExtensionSupport(const VkPhysicalDevice &physicalDevice);

    void updateUniformBuffer(uint32_t currentImage);

    void record_draw_command(const VkCommandBuffer &command_buffer,
                             DescriptorSet &descriptor_set,
                             const ShapeTypeEncoding shape_type_encoding,
                             const VertexBuffer &vertex_buffer,
                             const IndexBuffer &index_buffer, const size_t num_instances);

  public:
    GeometryPipeline(Window &window, std::shared_ptr<CoreGraphicsContext> ctx,
                     SwapChainManager &swap_chain_manager,
                     std::vector<UniformBuffer> &uniform_buffers,
                     VkDescriptorSetLayout &descriptor_set_layout, Sampler &sampler,
                     Texture &texture);
    ~GeometryPipeline();

    StorageBuffer<StorageBufferObject> &get_circle_instance_buffer();
    StorageBuffer<StorageBufferObject> &get_triangle_instance_buffer();
    StorageBuffer<StorageBufferObject> &get_rectangle_instance_buffer();
    StorageBuffer<StorageBufferObject> &get_hexagon_instance_buffer();

    // TODO: These render function should merge into one generic call
    void render_circles(const VkCommandBuffer &command_buffer);
    void render_triangles(const VkCommandBuffer &command_buffer);
    void render_rectangles(const VkCommandBuffer &command_buffer);
    void render_hexagons(const VkCommandBuffer &command_buffer);
};
