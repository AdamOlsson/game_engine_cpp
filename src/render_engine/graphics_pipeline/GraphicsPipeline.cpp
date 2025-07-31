#include "GraphicsPipeline.h"

graphics_pipeline::GraphicsPipeline::GraphicsPipeline(Pipeline &&pipeline)
    : m_pipeline(std::move(pipeline)) {}

// TODO: The descriptor set should be a class member
// TODO: Use wrapped classes
void graphics_pipeline::GraphicsPipeline::render(const VkCommandBuffer &command_buffer,
                                                 const VertexBuffer &vertex_buffer,
                                                 const IndexBuffer &index_buffer,
                                                 const VkDescriptorSet &descriptor_set,
                                                 const size_t num_instances) {

    const VkDeviceSize vertex_buffers_offset = 0;
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      m_pipeline.m_pipeline);

    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_pipeline.m_pipeline_layout, 0, 1, &descriptor_set, 0,
                            nullptr);

    vkCmdBindVertexBuffers(command_buffer, 0, 1, &vertex_buffer.buffer,
                           &vertex_buffers_offset);
    vkCmdBindIndexBuffer(command_buffer, index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(command_buffer, index_buffer.num_indices, num_instances, 0, 0, 0);
}
