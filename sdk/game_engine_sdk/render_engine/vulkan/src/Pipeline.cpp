#include "vulkan/Pipeline.h"
#include "vulkan/Vertex.h"
#include "vulkan/vulkan_core.h"

vulkan::Pipeline::Pipeline(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                           PipelineLayout &layout,
                           const ShaderModule &vertex_shader_module,
                           const ShaderModule &fragment_shader_module,
                           const PipelineOpts &&opts)
    : m_ctx(ctx),
      m_pipeline(create_graphics_pipeline(&layout, vertex_shader_module,
                                          fragment_shader_module, std::move(opts))) {}

vulkan::Pipeline::~Pipeline() {
    if (m_pipeline == VK_NULL_HANDLE) {
        return;
    }
    vkDestroyPipeline(m_ctx->logical_device, m_pipeline, nullptr);
    m_pipeline = VK_NULL_HANDLE;
}

vulkan::Pipeline::Pipeline(Pipeline &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), m_pipeline(other.m_pipeline) {
    other.m_pipeline = VK_NULL_HANDLE;
}

vulkan::Pipeline &vulkan::Pipeline::operator=(Pipeline &&other) noexcept {
    if (this != &other) {
        if (m_ctx && m_pipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(m_ctx->logical_device, m_pipeline, nullptr);
        }
        m_ctx = std::move(other.m_ctx);
        m_pipeline = other.m_pipeline;

        other.m_pipeline = VK_NULL_HANDLE;
    }
    return *this;
}

VkPipeline vulkan::Pipeline::create_graphics_pipeline(
    PipelineLayout *layout, const ShaderModule &vertex_shader_module,
    const ShaderModule &fragment_shader_module, const PipelineOpts &&opts) {

    VkPipelineShaderStageCreateInfo vertex_shader_stage_info{};
    vertex_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_stage_info.module = vertex_shader_module;
    vertex_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo fragment_shader_stage_info{};
    fragment_shader_stage_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_shader_stage_info.module = fragment_shader_module;
    fragment_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertex_shader_stage_info,
                                                      fragment_shader_stage_info};

    VkVertexInputBindingDescription binding_description =
        opts.vertex_input_info.binding_description.has_value()
            ? opts.vertex_input_info.binding_description.value()
            : Vertex::get_binding_description();

    auto attribute_descriptions = Vertex::get_attribute_descriptions();
    VkVertexInputAttributeDescription *attribute_descriptions_ptr = nullptr;
    uint32_t attribute_descriptions_count = 0;
    if (opts.vertex_input_info.attribute_descriptions) {
        attribute_descriptions_ptr = opts.vertex_input_info.attribute_descriptions;
        attribute_descriptions_count =
            opts.vertex_input_info.attribute_descriptions_count;
    } else {
        attribute_descriptions_ptr = attribute_descriptions.data();
        attribute_descriptions_count = attribute_descriptions.size();
    }

    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.vertexAttributeDescriptionCount = attribute_descriptions_count;
    vertex_input_info.pVertexBindingDescriptions = &binding_description;
    vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions_ptr;

    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = opts.assembler.topology;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = opts.viewport.width;
    viewport.height = opts.viewport.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = opts.scissor.extent;

    std::vector<VkDynamicState> dynamic_states = opts.dynamic_states.states;
    VkPipelineDynamicStateCreateInfo dynamic_state{};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();

    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    // Using any mode other than fill requires enabling a GPU feature.
    rasterizer.polygonMode = opts.rasterizer.polygon_mode;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = opts.rasterizer.cull_mode;
    rasterizer.frontFace = opts.rasterizer.front_face;
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

    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_TRUE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blending{};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;
    color_blending.blendConstants[0] = 0.0f; // Optional
    color_blending.blendConstants[1] = 0.0f; // Optional
    color_blending.blendConstants[2] = 0.0f; // Optional
    color_blending.blendConstants[3] = 0.0f; // Optional

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.renderPass = opts.pipeline_info.render_pass;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertex_input_info;
    pipelineInfo.pInputAssemblyState = &input_assembly;
    pipelineInfo.pViewportState = &viewport_state;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &color_blending;
    pipelineInfo.pDynamicState = &dynamic_state;
    pipelineInfo.layout = *layout;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;              // Optional

    VkPipeline graphicsPipeline;
    if (vkCreateGraphicsPipelines(m_ctx->logical_device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                  nullptr, &graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    // Note from tutorial:
    // The compilation and linking of the SPIR-V bytecode to machine code for
    // execution by the GPU doesn't happen until the graphics pipeline is
    // created. That means that we're allowed to destroy the shader modules
    // again as soon as pipeline creation is finished
    return graphicsPipeline;
}
