#include "render_engine/GraphicsPipeline.h"
#include "CoreGraphicsContext.h"
#include "glm/fwd.hpp"
#include "shape.h"

#include "render_engine/buffers/IndexBuffer.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/buffers/UniformBuffer.h"
#include "util.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>

static std::vector<char> readFile(const std::string filename) {
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

VkShaderModule createShaderModule(VkDevice &device, const std::vector<char> &code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void cleanupSwapChainFrameBuffers(VkDevice &device,
                                  std::vector<VkFramebuffer> &swapChainFramebuffers) {
    for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
        vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
    }
}

std::tuple<std::vector<VkSemaphore>, std::vector<VkSemaphore>, std::vector<VkFence>>
createSyncObjects(VkDevice &device, const int capacity) {
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    imageAvailableSemaphores.resize(capacity);
    renderFinishedSemaphores.resize(capacity);
    inFlightFences.resize(capacity);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < capacity; i++) {
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                              &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                              &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) !=
                VK_SUCCESS) {

            throw std::runtime_error(
                "failed to create synchronization objects for a frame!");
        }
    }
    return std::make_tuple(imageAvailableSemaphores, renderFinishedSemaphores,
                           inFlightFences);
}

VkRenderPass createRenderPass(VkDevice &device, VkFormat &swapChainImageFormat) {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkRenderPass renderPass;
    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
    return renderPass;
}

std::vector<VkDescriptorSet>
createDescriptorSets(VkDevice &device, VkDescriptorSetLayout &descriptorSetLayout,
                     VkDescriptorPool &descriptorPool, const int capacity,
                     const VkDeviceSize offset,
                     std::vector<std::unique_ptr<StorageBuffer>> &storage_buffers,
                     std::vector<std::unique_ptr<UniformBuffer>> &uniform_buffers) {

    std::vector<VkDescriptorSetLayout> layouts(capacity, descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(capacity);
    allocInfo.pSetLayouts = layouts.data();

    std::vector<VkDescriptorSet> descriptorSets;
    descriptorSets.resize(capacity);
    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < capacity; i++) {
        VkDescriptorBufferInfo storageBufferInfo{};
        storageBufferInfo.buffer = storage_buffers[i]->buffer;
        storageBufferInfo.offset = offset;
        storageBufferInfo.range = storage_buffers[i]->size;

        VkWriteDescriptorSet storageBufferDescriptorWrite{};
        storageBufferDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        storageBufferDescriptorWrite.dstSet = descriptorSets[i];
        storageBufferDescriptorWrite.dstBinding = 0;
        storageBufferDescriptorWrite.dstArrayElement = 0;
        storageBufferDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        storageBufferDescriptorWrite.descriptorCount = 1;
        storageBufferDescriptorWrite.pBufferInfo = &storageBufferInfo;
        storageBufferDescriptorWrite.pImageInfo = nullptr;       // Optional
        storageBufferDescriptorWrite.pTexelBufferView = nullptr; // Optional

        VkDescriptorBufferInfo uniformBufferInfo{};
        uniformBufferInfo.buffer = uniform_buffers[i]->buffer;
        uniformBufferInfo.offset = 0;
        uniformBufferInfo.range = uniform_buffers[i]->size;

        VkWriteDescriptorSet uniformBufferDescriptorWrite{};
        uniformBufferDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uniformBufferDescriptorWrite.dstSet = descriptorSets[i];
        uniformBufferDescriptorWrite.dstBinding = 1;
        uniformBufferDescriptorWrite.dstArrayElement = 0;
        uniformBufferDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferDescriptorWrite.descriptorCount = 1;
        uniformBufferDescriptorWrite.pBufferInfo = &uniformBufferInfo;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites = {
            storageBufferDescriptorWrite, uniformBufferDescriptorWrite};

        vkUpdateDescriptorSets(device, descriptorWrites.size(), descriptorWrites.data(),
                               0, nullptr);
    }
    return descriptorSets;
}

VkPipeline createGraphicsPipeline(VkDevice &device, const std::string vertex_shader_path,
                                  const std::string fragment_shader_path,
                                  VkDescriptorSetLayout &descriptorSetLayout,
                                  VkPipelineLayout &pipelineLayout,
                                  VkRenderPass &renderPass, SwapChain &swapChain) {

    auto vertShaderCode = readFile(vertex_shader_path);
    auto fragShaderCode = readFile(fragment_shader_path);

    VkShaderModule vertShaderModule = createShaderModule(device, vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(device, fragShaderCode);

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
    viewport.width = (float)swapChain.swapChainExtent.width;
    viewport.height = (float)swapChain.swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChain.swapChainExtent;

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
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
    return graphicsPipeline;
}

GraphicsPipeline::GraphicsPipeline(Window &window, CoreGraphicsContext &ctx)
    : ctx(&ctx),
      swapChain(SwapChain(ctx.physicalDevice, ctx.device, ctx.surface, *window.window)) {

    auto [_graphicsQueue, _presentQueue] = ctx.get_device_queues();
    graphicsQueue = _graphicsQueue;
    presentQueue = _presentQueue;

    renderPass = createRenderPass(ctx.device, swapChain.swapChainImageFormat);
    swapChainFramebuffers = swapChain.createFramebuffers(renderPass);

    auto storageBufferSetLayoutBinding =
        StorageBuffer::createDescriptorSetLayoutBinding(0);

    auto uniformBufferSetLayoutBinding =
        UniformBuffer::createDescriptorSetLayoutBinding(1);

    std::vector<VkDescriptorSetLayoutBinding> layoutBindings = {
        *storageBufferSetLayoutBinding, *uniformBufferSetLayoutBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = layoutBindings.size();
    layoutInfo.pBindings = layoutBindings.data();

    if (vkCreateDescriptorSetLayout(ctx.device, &layoutInfo, nullptr,
                                    &bufferDescriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    triangleGraphicsPipeline = createGraphicsPipeline(
        ctx.device, "src/render_engine/shaders/vert.spv",
        "src/render_engine/shaders/frag.spv", bufferDescriptorSetLayout,
        trianglePipelineLayout, renderPass, swapChain);

    rectangleGraphicsPipeline = createGraphicsPipeline(
        ctx.device, "src/render_engine/shaders/vert.spv",
        "src/render_engine/shaders/frag.spv", bufferDescriptorSetLayout,
        rectanglePipelineLayout, renderPass, swapChain);

    commandPool = createCommandPool(ctx.physicalDevice, ctx.device, ctx.surface);

    triangleVertexBuffer =
        createVertexBuffer(ctx.physicalDevice, ctx.device, Geometry::triangle_vertices,
                           commandPool, graphicsQueue);
    triangleIndexBuffer =
        createIndexBuffer(ctx.physicalDevice, ctx.device, Geometry::triangle_indices,
                          commandPool, graphicsQueue);

    rectangleVertexBuffer =
        createVertexBuffer(ctx.physicalDevice, ctx.device, Geometry::rectangle_vertices,
                           commandPool, graphicsQueue);
    rectangleIndexBuffer =
        createIndexBuffer(ctx.physicalDevice, ctx.device, Geometry::rectangle_indices,
                          commandPool, graphicsQueue);

    triangleInstanceBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    rectangleInstanceBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    size_t size = 1024;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        triangleInstanceBuffers[i] =
            createStorageBuffer(ctx.physicalDevice, ctx.device, size);
        rectangleInstanceBuffers[i] =
            createStorageBuffer(ctx.physicalDevice, ctx.device, size);
        uniformBuffers[i] = createUniformBuffer(ctx.physicalDevice, ctx.device,
                                                sizeof(UniformBufferObject));
    }

    auto [width, height] = window.dimensions();
    UniformBufferObject ubo{
        .dimensions = glm::vec2(static_cast<float>(width), static_cast<float>(height))};
    uniformBuffers[0]->updateUniformBuffer(ubo);
    uniformBuffers[1]->updateUniformBuffer(ubo);

    triangleDescriptorPool = createDescriptorPool(ctx.device, MAX_FRAMES_IN_FLIGHT);
    rectangleDescriptorPool = createDescriptorPool(ctx.device, MAX_FRAMES_IN_FLIGHT);

    triangleDescriptorSets = createDescriptorSets(
        ctx.device, bufferDescriptorSetLayout, triangleDescriptorPool,
        MAX_FRAMES_IN_FLIGHT, 0, triangleInstanceBuffers, uniformBuffers);
    rectangleDescriptorSets = createDescriptorSets(
        ctx.device, bufferDescriptorSetLayout, rectangleDescriptorPool,
        MAX_FRAMES_IN_FLIGHT, 0, rectangleInstanceBuffers, uniformBuffers);

    commandBuffers = createCommandBuffers(ctx.device, MAX_FRAMES_IN_FLIGHT);

    auto [_imageAvailableSemaphores, _renderFinishedSemaphores, _inFlightFences] =
        createSyncObjects(ctx.device, MAX_FRAMES_IN_FLIGHT);
    imageAvailableSemaphores = _imageAvailableSemaphores;
    renderFinishedSemaphores = _renderFinishedSemaphores;
    inFlightFences = _inFlightFences;
}

GraphicsPipeline::~GraphicsPipeline() {
    // Order of these functions matter
    cleanupSwapChainFrameBuffers(ctx->device, swapChainFramebuffers);
    swapChain.cleanup();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkUnmapMemory(ctx->device, triangleInstanceBuffers[i]->bufferMemory);
        vkFreeMemory(ctx->device, triangleInstanceBuffers[i]->bufferMemory, nullptr);
        vkDestroyBuffer(ctx->device, triangleInstanceBuffers[i]->buffer, nullptr);

        vkUnmapMemory(ctx->device, rectangleInstanceBuffers[i]->bufferMemory);
        vkFreeMemory(ctx->device, rectangleInstanceBuffers[i]->bufferMemory, nullptr);
        vkDestroyBuffer(ctx->device, rectangleInstanceBuffers[i]->buffer, nullptr);

        vkUnmapMemory(ctx->device, uniformBuffers[i]->bufferMemory);
        vkFreeMemory(ctx->device, uniformBuffers[i]->bufferMemory, nullptr);
        vkDestroyBuffer(ctx->device, uniformBuffers[i]->buffer, nullptr);
    }

    vkDestroyDescriptorPool(ctx->device, triangleDescriptorPool, nullptr);
    vkDestroyDescriptorPool(ctx->device, rectangleDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(ctx->device, bufferDescriptorSetLayout, nullptr);

    delete triangleIndexBuffer.release();
    delete triangleVertexBuffer.release();
    delete rectangleIndexBuffer.release();
    delete rectangleVertexBuffer.release();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(ctx->device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(ctx->device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(ctx->device, inFlightFences[i], nullptr);
    }
    vkDestroyCommandPool(ctx->device, commandPool, nullptr);

    vkDestroyPipeline(ctx->device, triangleGraphicsPipeline, nullptr);
    vkDestroyPipeline(ctx->device, rectangleGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(ctx->device, trianglePipelineLayout, nullptr);
    vkDestroyPipelineLayout(ctx->device, rectanglePipelineLayout, nullptr);
    vkDestroyRenderPass(ctx->device, renderPass, nullptr);
}

void GraphicsPipeline::render(
    Window &window, const std::vector<StorageBufferObject> &triangle_instance_data,
    const std::vector<StorageBufferObject> &rectangle_instance_data) {
    vkWaitForFences(ctx->device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(ctx->device, swapChain.swapChain, UINT64_MAX,
                                            imageAvailableSemaphores[currentFrame],
                                            VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain(ctx->physicalDevice, ctx->device, ctx->surface, *window.window);
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(ctx->device, 1, &inFlightFences[currentFrame]);
    vkResetCommandBuffer(commandBuffers[currentFrame], 0);

    uint32_t num_triangle_instances = triangle_instance_data.size();
    uint32_t num_rectangle_instances = rectangle_instance_data.size();

    triangleInstanceBuffers[currentFrame]->updateStorageBuffer(triangle_instance_data);
    rectangleInstanceBuffers[currentFrame]->updateStorageBuffer(rectangle_instance_data);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                  // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional
    if (vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChain.swapChainExtent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChain.swapChainExtent.width);
    viewport.height = static_cast<float>(swapChain.swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffers[currentFrame], 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChain.swapChainExtent;
    vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &scissor);

    if (num_triangle_instances > 0) {
        VkDeviceSize triangleVertexBuffersOffset = 0;
        uint32_t target_shape = ShapeTypeEncoding::TriangleShape;
        vkCmdBindPipeline(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS,
                          triangleGraphicsPipeline);
        vkCmdBindDescriptorSets(commandBuffers[currentFrame],
                                VK_PIPELINE_BIND_POINT_GRAPHICS, trianglePipelineLayout,
                                0, 1, &triangleDescriptorSets[currentFrame], 0, nullptr);
        vkCmdPushConstants(commandBuffers[currentFrame], trianglePipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(uint32_t),
                           &target_shape);

        vkCmdBindVertexBuffers(commandBuffers[currentFrame], 0, 1,
                               &triangleVertexBuffer->buffer,
                               &triangleVertexBuffersOffset);
        vkCmdBindIndexBuffer(commandBuffers[currentFrame], triangleIndexBuffer->buffer, 0,
                             VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(commandBuffers[currentFrame],
                         static_cast<uint32_t>(Geometry::triangle_indices.size()),
                         num_triangle_instances, 0, 0, 0);
    }

    if (num_rectangle_instances > 0) {
        uint32_t target_shape = ShapeTypeEncoding::RectangleShape;
        VkDeviceSize rectangleOffset = 0;
        vkCmdBindPipeline(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS,
                          rectangleGraphicsPipeline);
        vkCmdBindDescriptorSets(commandBuffers[currentFrame],
                                VK_PIPELINE_BIND_POINT_GRAPHICS, rectanglePipelineLayout,
                                0, 1, &rectangleDescriptorSets[currentFrame], 0, nullptr);
        vkCmdPushConstants(commandBuffers[currentFrame], rectanglePipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(uint32_t),
                           &target_shape);
        vkCmdBindVertexBuffers(commandBuffers[currentFrame], 0, 1,
                               &rectangleVertexBuffer->buffer, &rectangleOffset);
        vkCmdBindIndexBuffer(commandBuffers[currentFrame], rectangleIndexBuffer->buffer,
                             0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(commandBuffers[currentFrame],
                         static_cast<uint32_t>(Geometry::rectangle_indices.size()),
                         num_rectangle_instances, 0, 0, 0);
    }

    vkCmdEndRenderPass(commandBuffers[currentFrame]);
    if (vkEndCommandBuffer(commandBuffers[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain.swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    // TODO: framebufferResized needs to be set from the window callback when the
    // window is resized
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        framebufferResized) {
        recreateSwapChain(ctx->physicalDevice, ctx->device, ctx->surface, *window.window);

        // It is important to do this after vkQueuePresentKHR to ensure that the
        // semaphores are in a consistent state, otherwise a signaled semaphore
        // may never be properly waited upon.
        framebufferResized = false;
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void GraphicsPipeline::recreateSwapChain(VkPhysicalDevice &physicalDevice,
                                         VkDevice &device, VkSurfaceKHR &surface,
                                         GLFWwindow &window) {
    // All execution is paused when the window is minimized
    int width = 0, height = 0;
    glfwGetFramebufferSize(&window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(&window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device);
    cleanupSwapChainFrameBuffers(device, swapChainFramebuffers);
    swapChain.cleanup(); // No need to explicitly cleanup
    swapChain = SwapChain(physicalDevice, device, surface, window);
    swapChainFramebuffers = swapChain.createFramebuffers(renderPass);
}

std::vector<VkImageView>
GraphicsPipeline::createImageViews(VkDevice &device,
                                   std::vector<VkImage> &swapChainImages) {
    std::vector<VkImageView> swapChainImageViews;
    swapChainImageViews.resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChain.swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
    return swapChainImageViews;
}

std::vector<VkCommandBuffer> GraphicsPipeline::createCommandBuffers(VkDevice &device,
                                                                    const int capacity) {
    std::vector<VkCommandBuffer> commandBuffers;
    commandBuffers.resize(capacity);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    return commandBuffers;
}

VkCommandPool GraphicsPipeline::createCommandPool(VkPhysicalDevice &physicalDevice,
                                                  VkDevice &device,
                                                  VkSurfaceKHR &surface) {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice, surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    VkCommandPool commandPool;
    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
    return commandPool;
}

VkDescriptorPool GraphicsPipeline::createDescriptorPool(VkDevice &device,
                                                        const int capacity) {
    std::array<VkDescriptorPoolSize, 2> poolSizes{};

    poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(capacity);

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(capacity);

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
