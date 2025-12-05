#pragma once

#include "PipelineLayout.h"
#include "ShaderModule.h"
#include "SwapChainManager.h"
#include "context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>

namespace vulkan {

struct PipelineOpts {
    struct {
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    } assembler;
    struct {
        VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL;
    } rasterizer;
};

class Pipeline {
  private:
    std::shared_ptr<context::GraphicsContext> m_ctx;

    VkPipeline m_pipeline;

    VkPipeline create_graphics_pipeline(PipelineLayout *layout,
                                        const ShaderModule &vertex_shader_module,
                                        const ShaderModule &fragment_shader_module,
                                        SwapChainManager &swap_chain_manager,
                                        const PipelineOpts &&opts);

  public:
    Pipeline() = default;
    Pipeline(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
             PipelineLayout &layout, const ShaderModule &vertex_shader_module,
             const ShaderModule &fragment_shader_module,
             SwapChainManager &swap_chain_manager, const PipelineOpts &&opts);

    ~Pipeline();

    Pipeline(Pipeline &&other) noexcept;
    Pipeline &operator=(Pipeline &&other) noexcept;

    Pipeline &operator=(const Pipeline &other) = delete;
    Pipeline(const Pipeline &other) = delete;

    operator VkPipeline() const { return m_pipeline; }
};
} // namespace vulkan
