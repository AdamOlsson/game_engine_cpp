#include "RenderPass.h"
#include <stdexcept>

RenderPass::RenderPass(vulkan::CommandBuffer &command_buffer)
    : m_command_buffer(command_buffer) {}

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
    viewport.width = static_cast<float>(dim.width);
    viewport.height = static_cast<float>(dim.height);
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
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkCommandBuffer command_buffer = m_command_buffer;
    submitInfo.pCommandBuffers = &command_buffer;

    VkSemaphore waitSemaphores[] = {m_image_available};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;

    VkSemaphore signalSemaphores[] = {m_submit_completed};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_device_queues.graphics_queue, 1, &submitInfo,
                      m_in_flight_fence) != VK_SUCCESS) {
        throw std::runtime_error("RenderPass::Failed to submit draw command buffer!");
    }
}

VkResult RenderPass::present() {
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_submit_completed;

    VkSwapchainKHR swapChains[] = {m_swap_chain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_image_index;
    presentInfo.pResults = nullptr; // Optional

    VkResult result = vkQueuePresentKHR(m_device_queues.present_queue, &presentInfo);
    return result;
}

VkResult RenderPass::end_submit_present() {
    end();
    submit();
    return present();
}
