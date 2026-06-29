#include "ri_components/ri_components.h"
#include "ri_platform/ri_platform.h"
#include "ri_renderer/ri_renderer.h"
#include "ruin.h"
#include <stdint.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>


void record_command(RI_Renderer *r, RI_Platform *p, VkCommandBuffer cmd, uint32_t img_idx) {
    
    VkCommandBufferBeginInfo cbi = {0};
    cbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    RI_Renderer_Pipeline *test = &r->pipelines.test_pipeline;

    VkClearValue clear = {0};
    clear.color.float32[0] = 0.8f;
    clear.color.float32[1] = 0.8f;
    clear.color.float32[2] = 0.8f;
    clear.color.float32[3] = 1.0f;

    vkResetCommandBuffer(cmd, 0);

    if (vkBeginCommandBuffer(cmd, &cbi) != VK_SUCCESS) {
        printf("Failed to begin command buffer #%d\n", img_idx);
        return;
    }

    VkRenderPassBeginInfo rpi = {0};
    rpi.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpi.renderPass        = r->renderpasses.present_pass;
    rpi.framebuffer       = r->swapchain.swapchain_framebuffers[img_idx];
    rpi.renderArea.offset = (VkOffset2D){ 0, 0};
    rpi.renderArea.extent = r->swapchain.swapchain_extent;
    rpi.pClearValues      = &clear;
    rpi.clearValueCount   = 1;

    VkViewport vw = (VkViewport){
        .x = 0,
        .y = 0,
        .width  = p->active_config.width,
        .height = p->active_config.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = (VkRect2D){
        .extent = r->swapchain.swapchain_extent,
        .offset.x = 0,
        .offset.y = 0
    };

    vkCmdBeginRenderPass(cmd, &rpi, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmd, test->bind_point, test->pipeline);

    vkCmdSetViewport(cmd, 0, 1, &vw);
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    vkCmdDraw(cmd, 3, 1, 0, 0);

    vkCmdEndRenderPass(cmd);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
        printf("Failed to record command #%d\n", img_idx);
        return;
    }
}





void record_command_2d(RI_Renderer *r, RI_Platform *p, RI_Components *c, VkCommandBuffer cmd, uint32_t img_idx) {
    VkCommandBufferBeginInfo cbi = {0};
    cbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    RI_Renderer_Pipeline *p_bindless_2d = &r->pipelines.bindless_pipeline_2d;

    VkClearValue clear = {0};
    clear.color.float32[0] = 0.0f;
    clear.color.float32[1] = 0.5f;
    clear.color.float32[2] = 1.0f;
    clear.color.float32[3] = 1.0f;


    VkRenderPassBeginInfo rbi = {0};
    rbi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rbi.renderPass = r->renderpasses.present_pass;
    rbi.framebuffer = r->swapchain.swapchain_framebuffers[img_idx];
    rbi.pClearValues = &clear;
    rbi.clearValueCount = 1;
    rbi.renderArea = (VkRect2D){
        .offset = {0},
        .extent = r->swapchain.swapchain_extent
    };

    VkViewport vw = (VkViewport) {
        .x = 0, .y = 0,
        .width  = p->active_config.width,
        .height = p->active_config.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D sc = (VkRect2D) {
        .offset = {0},
        .extent = r->swapchain.swapchain_extent
    };

    vkResetCommandBuffer(cmd, 0);

    if (vkBeginCommandBuffer(cmd, &cbi) != VK_SUCCESS) {
        printf("Failed to begin command buffer.\n");
        return;
    }

    vkCmdBeginRenderPass(cmd, &rbi, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdSetViewport(cmd, 0, 1, &vw);
        vkCmdSetScissor(cmd, 0, 1, &sc);
        vkCmdBindPipeline(cmd, p_bindless_2d->bind_point, p_bindless_2d->pipeline);
        vkCmdBindDescriptorSets(cmd, p_bindless_2d->bind_point, p_bindless_2d->layout, 0, 2, p_bindless_2d->d_sets, 0, NULL);

        uint32_t instance_count = c->sprite_renderers.dense_indices.len;

        if (instance_count > 0) {
            vkCmdDraw(cmd, 6, instance_count, 0, 0);
        }

    vkCmdEndRenderPass(cmd);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
        printf("Failed to end command buffer.\n");
        return;
    }
}





