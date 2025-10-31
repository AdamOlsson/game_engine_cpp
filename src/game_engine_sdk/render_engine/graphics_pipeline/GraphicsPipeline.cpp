#include "game_engine_sdk/render_engine/graphics_pipeline/GraphicsPipeline.h"

#include <fstream>
#include <stdexcept>
#include <vector>

graphics_pipeline::GraphicsPipeline::GraphicsPipeline(vulkan::PipelineLayout &&layout,
                                                      vulkan::Pipeline &&pipeline)
    : m_pipeline(std::move(pipeline)), m_pipeline_layout(std::move(layout)) {}

void graphics_pipeline::GraphicsPipeline::render(
    const VkCommandBuffer &command_buffer,
    const vulkan::buffers::VertexBuffer &vertex_buffer,
    const vulkan::buffers::IndexBuffer &index_buffer,
    const VkDescriptorSet &descriptor_set, const size_t num_instances) {

    const VkDeviceSize vertex_buffers_offset = 0;
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_pipeline_layout, 0, 1, &descriptor_set, 0, nullptr);

    vkCmdBindVertexBuffers(command_buffer, 0, 1, &vertex_buffer.buffer,
                           &vertex_buffers_offset);
    vkCmdBindIndexBuffer(command_buffer, index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(command_buffer, index_buffer.num_indices, num_instances, 0, 0, 0);
}

std::vector<char> graphics_pipeline::readFile(const std::string filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error(std::format("Failed to open file {}", filename));
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}
