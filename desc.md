Here is a self-contained instruction set for a coding agent:

---

## Refactor: Game-Owned Render Passes

### Context

The codebase is a Vulkan-based game engine. Currently `SwapChain` creates and owns a single hardcoded `VkRenderPass` with `CLEAR`/`STORE` load/store ops. The goal is to move render pass creation and ownership up to the game layer so the game can define multiple passes with different ops, while renderers remain unaware of pass semantics.

---

### 1 — New File: `vulkan/RenderPassOpts.h`

Create this file from scratch:

```cpp
#pragma once
#include <vulkan/vulkan.h>

namespace vulkan {

struct RenderPassOpts {
    struct Color {
        VkAttachmentLoadOp  load_op        = VK_ATTACHMENT_LOAD_OP_CLEAR;
        VkAttachmentStoreOp store_op       = VK_ATTACHMENT_STORE_OP_STORE;
        VkImageLayout       initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout       final_layout   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    } color;
};

} // namespace vulkan
```

---

### 2 — New File: `vulkan/RenderPassHandle.h`

Create this file from scratch. It is a move-only RAII wrapper around `VkRenderPass`:

```cpp
#pragma once
#include "vulkan/context/GraphicsContext.h"
#include <vulkan/vulkan.h>

namespace vulkan {

class RenderPassHandle {
  public:
    RenderPassHandle() = default;
    RenderPassHandle(std::shared_ptr<context::GraphicsContext> ctx, VkRenderPass pass)
        : m_ctx(ctx), m_pass(pass) {}

    ~RenderPassHandle() {
        if (m_pass != VK_NULL_HANDLE)
            vkDestroyRenderPass(m_ctx->logical_device, m_pass, nullptr);
    }

    RenderPassHandle(RenderPassHandle &&o) noexcept
        : m_ctx(std::move(o.m_ctx)), m_pass(o.m_pass) {
        o.m_pass = VK_NULL_HANDLE;
    }
    RenderPassHandle &operator=(RenderPassHandle &&o) noexcept {
        if (this != &o) {
            if (m_pass != VK_NULL_HANDLE)
                vkDestroyRenderPass(m_ctx->logical_device, m_pass, nullptr);
            m_ctx  = std::move(o.m_ctx);
            m_pass = o.m_pass;
            o.m_pass = VK_NULL_HANDLE;
        }
        return *this;
    }
    RenderPassHandle(const RenderPassHandle &) = delete;
    RenderPassHandle &operator=(const RenderPassHandle &) = delete;

    VkRenderPass get() const { return m_pass; }
    operator VkRenderPass() const { return m_pass; }

  private:
    std::shared_ptr<context::GraphicsContext> m_ctx;
    VkRenderPass m_pass = VK_NULL_HANDLE;
};

} // namespace vulkan
```

---

### 3 — Modify: `vulkan/SwapChain`

**Remove** from `SwapChain`:
- The `m_render_pass` member field
- The `create_render_pass(VkFormat &image_format)` private method in its current form
- All calls to `vkDestroyRenderPass` inside `destroy()` and `recreate_swap_chain()`
- The `vkDestroyRenderPass` call on `old_render_pass` inside `recreate_swap_chain()`

**Add** to `SwapChain`:
- A public method `VkRenderPass create_render_pass(VkFormat image_format, const RenderPassOpts &opts)` with this implementation:

```cpp
VkRenderPass vulkan::SwapChain::create_render_pass(VkFormat image_format,
                                                    const RenderPassOpts &opts) {
    VkAttachmentDescription color{};
    color.format         = image_format;
    color.samples        = VK_SAMPLE_COUNT_1_BIT;
    color.loadOp         = opts.color.load_op;
    color.storeOp        = opts.color.store_op;
    color.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color.initialLayout  = opts.color.initial_layout;
    color.finalLayout    = opts.color.final_layout;

    VkAttachmentReference color_ref{};
    color_ref.attachment = 0;
    color_ref.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &color_ref;

    VkSubpassDependency dep{};
    dep.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dep.dstSubpass    = 0;
    dep.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dep.srcAccessMask = opts.color.load_op == VK_ATTACHMENT_LOAD_OP_LOAD
                            ? VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
                            : 0;
    dep.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo info{};
    info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments    = &color;
    info.subpassCount    = 1;
    info.pSubpasses      = &subpass;
    info.dependencyCount = 1;
    info.pDependencies   = &dep;

    VkRenderPass render_pass;
    if (vkCreateRenderPass(m_ctx->logical_device, &info, nullptr, &render_pass)
            != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
    return render_pass;
}
```

**Modify** `setup()`: remove the line `m_render_pass = create_render_pass(surface_format.format)`. The framebuffer creation call that previously passed `m_render_pass` must now receive a `VkRenderPass` argument. Since framebuffers only need a compatible render pass at creation time, pass any render pass that shares the same format and sample count — the caller must supply this. Change `create_framebuffers()` to accept a `VkRenderPass` parameter and forward it to each `Framebuffer` constructor.

**Modify** `setup()` signature to accept an optional `VkRenderPass` for framebuffer compatibility:

```cpp
void setup(VkSwapchainKHR &old_swap_chain, VkRenderPass framebuffer_compatible_pass);
```

The caller (`SwapChainManager`) is responsible for creating a pass before calling `setup()` on recreation, and passing it in.

---

### 4 — Modify: `vulkan/SwapChainManager`

**Add** a public method that delegates to `SwapChain`:

```cpp
vulkan::RenderPassHandle create_render_pass(const vulkan::RenderPassOpts &opts) {
    return vulkan::RenderPassHandle(
        m_ctx,
        m_swap_chain.create_render_pass(m_swap_chain.get_surface_format(), opts));
}
```

`SwapChain` must expose `get_surface_format()` returning the `VkFormat` chosen during setup. Store it as `m_surface_format` during `setup()`.

**Add** a `begin_render_pass` method that starts a render pass against the current framebuffer:

```cpp
vulkan::RenderPass begin_render_pass(const vulkan::CommandBuffer &cmd,
                                     VkRenderPass render_pass);
```

This replaces the current `get_render_pass()`. It calls `vkCmdBeginRenderPass` using the current swap chain framebuffer and the provided `VkRenderPass`. The returned `RenderPass` RAII object calls `vkCmdEndRenderPass` on `.end()` and additionally calls `vkQueueSubmit` + `vkQueuePresentKHR` on `.end_submit_present()`.

---

### 5 — Modify: `GeometryPipeline` constructor

Add a `VkRenderPass target_render_pass` parameter as the last argument. Replace the line:

```cpp
.render_pass = swap_chain_manager->m_swap_chain.m_render_pass,
```

with:

```cpp
.render_pass = target_render_pass,
```

`GeometryPipeline` no longer reads any render pass from `SwapChainManager` directly.

Apply the same change to `QuadRenderer` and `TextRenderer` pipeline constructors if they follow the same pattern.

---

### 6 — Modify: `CaravanDefence::setup()`

Add two `RenderPassHandle` members to the class:

```cpp
vulkan::RenderPassHandle m_render_pass;
```

In `setup()`, after constructing `m_swap_chain_manager`, create both passes before constructing any renderer:

```cpp
m_render_pass = m_swap_chain_manager->create_render_pass({
    .color = {
        .load_op        = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .store_op       = VK_ATTACHMENT_STORE_OP_STORE,
        .initial_layout = VK_IMAGE_LAYOUT_UNDEFINED,
        .final_layout   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    }
});

```

Pass `m_render_pass.get()` to renderers at their construction sites.

---

### 7 — Modify: `CaravanDefence::render()`

Replace the single `get_render_pass()` call with two sequenced passes. Only the final pass submits and presents:

```cpp
void render() override {
    auto cmd = m_command_buffer_manager->get_command_buffer();

    auto world_pass = m_swap_chain_manager->begin_render_pass(cmd, m_world_pass.get());
    m_geom_renderer->sync_render_slots();
    m_quad_renderer->sync_render_slots();
    m_quad_renderer->render(cmd, &world_vp, 256);
    m_geom_renderer->render(cmd, &world_vp, 256);
    world_pass.end();
}
```

---

### Invariants the Agent Must Preserve

- `SwapChain` must not store any `VkRenderPass` member after this refactor
- `GeometryPipeline`, `QuadRenderer`, and `TextRenderer` must not read render passes from `SwapChainManager` directly — only accept them as constructor arguments
- `vkDestroyRenderPass` must only be called from `RenderPassHandle`'s destructor
- Framebuffers are shared across passes and must not be duplicated
- Both render passes are recorded into the same `VkCommandBuffer` before any submission occurs — `world_pass.end()` must not submit
