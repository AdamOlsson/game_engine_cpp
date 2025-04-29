#include "render_engine/GraphicsPipeline.h"
#include "CoreGraphicsContext.h"
#include "render_engine/CommandHandler.h"
#include "render_engine/DescriptorSet.h"
#include "render_engine/RenderableGeometry.h"
#include "render_engine/Sampler.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/buffers/UniformBuffer.h"
#include "render_engine/resources/ResourceManager.h"
#include "util.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>

GraphicsPipeline::GraphicsPipeline(Window &window,
                                   std::shared_ptr<CoreGraphicsContext> ctx,
                                   CommandHandler &command_handler, SwapChain &swap_chain,
                                   VkRenderPass &render_pass,
                                   std::vector<UniformBuffer> &uniform_buffers,
                                   VkDescriptorSetLayout &descriptor_set_layout,
                                   Sampler &sampler, Texture &texture)
    : ctx(ctx) {

    auto [graphicsQueue, presentQueue] = ctx->get_device_queues();

    auto &resoure_manager = ResourceManager::get_instance();
    auto vert_shader_code = resoure_manager.get_resource<ShaderResource>("Vert");
    auto frag_shader_code =
        resoure_manager.get_resource<ShaderResource>("GeometryFragment");

    VkShaderModule vert_shader_module = createShaderModule(
        ctx->device, vert_shader_code->bytes(), vert_shader_code->length());

    VkShaderModule frag_shader_module = createShaderModule(
        ctx->device, frag_shader_code->bytes(), frag_shader_code->length());

    graphics_pipeline = createGraphicsPipeline(ctx->device, vert_shader_module,
                                               frag_shader_module, descriptor_set_layout,
                                               pipeline_layout, render_pass, swap_chain);

    vkDestroyShaderModule(ctx->device, vert_shader_module, nullptr);
    vkDestroyShaderModule(ctx->device, frag_shader_module, nullptr);

    VkCommandPool command_pool = command_handler.pool();

    const int num_geometries = 6;
    descriptor_pool =
        createDescriptorPool(ctx->device, MAX_FRAMES_IN_FLIGHT * num_geometries);

    auto circle_descriptor_set =
        DescriptorSet(ctx, descriptor_set_layout, descriptor_pool, MAX_FRAMES_IN_FLIGHT,
                      &uniform_buffers, &texture, &sampler);
    circle_geometry = std::make_unique<Geometry::Circle>(
        ctx, command_pool, graphicsQueue, std::move(circle_descriptor_set));

    auto triangle_descriptor_set =
        DescriptorSet(ctx, descriptor_set_layout, descriptor_pool, MAX_FRAMES_IN_FLIGHT,
                      &uniform_buffers, &texture, &sampler);
    triangle_geometry = std::make_unique<Geometry::Triangle>(
        ctx, command_pool, graphicsQueue, std::move(triangle_descriptor_set));

    auto rectangle_descriptor_set =
        DescriptorSet(ctx, descriptor_set_layout, descriptor_pool, MAX_FRAMES_IN_FLIGHT,
                      &uniform_buffers, &texture, &sampler);
    rectangle_geometry = std::make_unique<Geometry::Rectangle>(
        ctx, command_pool, graphicsQueue, std::move(rectangle_descriptor_set));

    auto hexagon_descriptor_set =
        DescriptorSet(ctx, descriptor_set_layout, descriptor_pool, MAX_FRAMES_IN_FLIGHT,
                      &uniform_buffers, &texture, &sampler);
    hexagon_geometry = std::make_unique<Geometry::Hexagon>(
        ctx, command_pool, graphicsQueue, std::move(hexagon_descriptor_set));
}

GraphicsPipeline::~GraphicsPipeline() {
    // Order of these functions matter
    vkDestroyDescriptorPool(ctx->device, descriptor_pool, nullptr);
    vkDestroyPipeline(ctx->device, graphics_pipeline, nullptr);
    vkDestroyPipelineLayout(ctx->device, pipeline_layout, nullptr);
}

void GraphicsPipeline::render_circles(
    const VkCommandBuffer &command_buffer,
    std::vector<StorageBufferObject> &&circle_instance_data) {
    circle_geometry->update_instance_buffer(
        std::forward<std::vector<StorageBufferObject>>(circle_instance_data));
    uint32_t num_instances = circle_instance_data.size();
    if (num_instances > 0) {
        circle_geometry->record_draw_command(command_buffer, graphics_pipeline,
                                             pipeline_layout, num_instances);
    }
}

void GraphicsPipeline::render_triangles(

    const VkCommandBuffer &command_buffer,
    std::vector<StorageBufferObject> &&triangle_instance_data) {
    triangle_geometry->update_instance_buffer(
        std::forward<std::vector<StorageBufferObject>>(triangle_instance_data));
    uint32_t num_instances = triangle_instance_data.size();
    if (num_instances > 0) {
        triangle_geometry->record_draw_command(command_buffer, graphics_pipeline,
                                               pipeline_layout, num_instances);
    }
}

void GraphicsPipeline::render_rectangles(
    const VkCommandBuffer &command_buffer,
    std::vector<StorageBufferObject> &&rectangle_instance_data) {
    rectangle_geometry->update_instance_buffer(
        std::forward<std::vector<StorageBufferObject>>(rectangle_instance_data));
    size_t num_instances = rectangle_instance_data.size();
    if (num_instances > 0) {
        rectangle_geometry->record_draw_command(command_buffer, graphics_pipeline,
                                                pipeline_layout, num_instances);
    }
}

void GraphicsPipeline::render_hexagons(
    const VkCommandBuffer &command_buffer,
    std::vector<StorageBufferObject> &&hexagon_instance_data) {
    hexagon_geometry->update_instance_buffer(
        std::forward<std::vector<StorageBufferObject>>(hexagon_instance_data));
    size_t num_instances = hexagon_instance_data.size();
    if (num_instances > 0) {
        hexagon_geometry->record_draw_command(command_buffer, graphics_pipeline,
                                              pipeline_layout, num_instances);
    }
}

VkDescriptorPool createDescriptorPool(VkDevice &device, const int capacity) {
    std::array<VkDescriptorPoolSize, 3> poolSizes{};

    poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(capacity);

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(capacity);

    // TODO: Do we really need this many samplers?
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[2].descriptorCount = static_cast<uint32_t>(capacity);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(capacity);

    VkDescriptorPool descriptorPool;
    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
    return descriptorPool;
}

VkPipeline createGraphicsPipeline(const VkDevice &device,
                                  const VkShaderModule vertShaderModule,
                                  const VkShaderModule fragShaderModule,
                                  VkDescriptorSetLayout &descriptorSetLayout,
                                  VkPipelineLayout &pipelineLayout,
                                  VkRenderPass &renderPass, SwapChain &swapChain) {

    // Note from tutorial:
    // There is one more (optional) member, pSpecializationInfo, which we won't
    // be using here, but is worth discussing. It allows you to specify values
    // for shader constants. You can use a single shader module where its
    // behavior can be configured at pipeline creation by specifying different
    // values for the constants used in it. This is more efficient than
    // configuring the shader using variables at render time, because the
    // compiler can do optimizations like eliminating if statements that depend
    // on these values

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                      fragShaderStageInfo};

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChain.m_extent.width;
    viewport.height = (float)swapChain.m_extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChain.m_extent;

    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                                 VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    // Using any mode other than fill requires enabling a GPU feature.
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f;          // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;          // Optional
    multisampling.pSampleMask = nullptr;            // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE;      // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(uint32_t);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &push_constant_range;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;              // Optional

    VkPipeline graphicsPipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
                                  &graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    // Note from tutorial:
    // The compilation and linking of the SPIR-V bytecode to machine code for
    // execution by the GPU doesn't happen until the graphics pipeline is
    // created. That means that we're allowed to destroy the shader modules
    // again as soon as pipeline creation is finished
    return graphicsPipeline;
}

VkShaderModule createShaderModule(const VkDevice &device, const uint8_t *data,
                                  const size_t len) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = len;
    createInfo.pCode = reinterpret_cast<const uint32_t *>(data);

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

std::vector<char> readFile(const std::string filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}
