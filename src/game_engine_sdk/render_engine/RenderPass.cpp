#include "game_engine_sdk/render_engine/RenderPass.h"
#include <stdexcept>

RenderPass::RenderPass(vulkan::CommandBuffer &command_buffer, SwapChain *swap_chain)
    : m_command_buffer(command_buffer), m_swap_chain(swap_chain) {}

void RenderPass::begin() {
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_render_pass;
    renderPassInfo.framebuffer = m_frame_buffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_render_area_extent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(m_command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::set_viewport(const Dimension &dim) {
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = dim.width();
    viewport.height = dim.height();
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(m_command_buffer, 0, 1, &viewport);
}

void RenderPass::set_scissor(const VkExtent2D &extent) {
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;
    vkCmdSetScissor(m_command_buffer, 0, 1, &scissor);
}

void RenderPass::end() {
    vkCmdEndRenderPass(m_command_buffer);
    m_command_buffer.end();
}

void RenderPass::submit() {
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkCommandBuffer command_buffer = m_command_buffer;
    submit_info.pCommandBuffers = &command_buffer;

    VkSemaphore waitSemaphores[] = {m_image_available};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = waitSemaphores;
    submit_info.pWaitDstStageMask = waitStages;
    submit_info.commandBufferCount = 1;

    VkSemaphore signalSemaphores[] = {m_submit_completed};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_device_queues.graphics_queue, 1, &submit_info,
                      m_in_flight_fence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }
}

void RenderPass::present() {
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &m_submit_completed;

    VkSwapchainKHR swapChains[] = {m_swap_chain->m_swap_chain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapChains;
    present_info.pImageIndices = &m_image_index;
    present_info.pResults = nullptr; // Optional

    VkResult result = vkQueuePresentKHR(m_device_queues.present_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        logger::error("Failed to present swap chain image");
        logger::info("Recreating swap chain");
        m_swap_chain->recreate_swap_chain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }
}

void RenderPass::end_submit_present() {
    end();
    submit();
    present();
}
