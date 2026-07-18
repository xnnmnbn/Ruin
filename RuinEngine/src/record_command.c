#include "cvec.h"
#include "ri_components/ri_components.h"
#include "ri_platform/ri_platform.h"
#include "ri_renderer/ri_renderer.h"
#include "ruin.h"
#include "ruin__internal.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>


void record_command(RuinInternal *engine, VkCommandBuffer cmd, uint32_t img_idx) {
    
    VkCommandBufferBeginInfo cbi = {0};
    cbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    RI_Renderer_Pipeline *test = &engine->renderer.pipelines.test_pipeline;

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
    rpi.renderPass        = engine->renderer.renderpasses.present_pass;
    rpi.framebuffer       = engine->renderer.swapchain.swapchain_framebuffers[img_idx];
    rpi.renderArea.offset = (VkOffset2D){ 0, 0};
    rpi.renderArea.extent = engine->renderer.swapchain.swapchain_extent;
    rpi.pClearValues      = &clear;
    rpi.clearValueCount   = 1;

    RnEntity c_use = engine->components.camera_in_use;

    RnCamera2D *c2 = &(from_void(engine->components.camera2ds.begin_points[0], RnCamera2D)[c_use]);
    RnCamera3D *c3 = &(from_void(engine->components.camera3ds.begin_points[0], RnCamera3D)[c_use]);

    uint32_t w = 0;
    uint32_t h = 0;

    if (engine->components.camera_mode == 2) {
        w = c2->width; h = c2->height;
    } else if (engine->components.camera_mode == 3) {
        w = c3->width; h = c3->height;
    }

    VkViewport vw = (VkViewport){
        .x = 0,
        .y = 0,
        .width  = w,
        .height = h,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = (VkRect2D){
        .extent = engine->renderer.swapchain.swapchain_extent,
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

    RI_Renderer_Pipeline *p_bindless_offscreen_2d = &r->pipelines.bindless_offscreen_2d;
    RI_Renderer_Pipeline *p_post_process          = &r->pipelines.post_process_pipeline;

    VkClearValue clear = {0};
    clear.color.float32[0] = 0.75f;
    clear.color.float32[1] = 0.0f;
    clear.color.float32[2] = 0.75f;
    clear.color.float32[3] = 1.0f;


    VkRenderPassBeginInfo rp_offscreen = {0};
    rp_offscreen.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_offscreen.renderPass = r->renderpasses.offscreen_pass;
    rp_offscreen.framebuffer = r->framebuffers.offscreen.framebuffer;
    rp_offscreen.pClearValues = &clear;
    rp_offscreen.clearValueCount = 1;
    rp_offscreen.renderArea = (VkRect2D){
        .offset = {0},
        .extent = r->framebuffers.offscreen.extent
    };

    VkRenderPassBeginInfo rp_present = {0};
    rp_present.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_present.renderPass = r->renderpasses.present_pass;
    rp_present.framebuffer = r->swapchain.swapchain_framebuffers[img_idx];
    rp_present.renderArea = (VkRect2D) {
        .offset = {0},
        .extent = r->swapchain.swapchain_extent
    };

    
    RnEntity c_use = c->camera_in_use;

    RnCamera2D *c2 = &(from_void(c->camera2ds.begin_points[0], RnCamera2D)[c_use]);
    RnCamera3D *c3 = &(from_void(c->camera3ds.begin_points[0], RnCamera3D)[c_use]);

    uint32_t w = 0;
    uint32_t h = 0;

    if (c->camera_mode == 2) {
        w = c2->width; h = c2->height;
    } else if (c->camera_mode == 3) {
        w = c3->width; h = c3->height;
    }
    
    VkViewport vw_offscreen = (VkViewport) {
        .x = 0, .y = 0,
        .width  = r->active_config.resolution_x,
        .height = r->active_config.resolution_y,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D sc_offscreen = (VkRect2D) {
        .offset = {0},
        .extent = (VkExtent2D) {
            .width = r->active_config.resolution_x,
            .height = r->active_config.resolution_y
        }
    };

    VkViewport vw_present = (VkViewport) {
        .x = 0, .y = 0,
        .width = r->swapchain.swapchain_extent.width,
        .height = r->swapchain.swapchain_extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D sc_present = (VkRect2D) {
        .offset = {0},
        .extent = r->swapchain.swapchain_extent
    };

    vkResetCommandBuffer(cmd, 0);

    if (vkBeginCommandBuffer(cmd, &cbi) != VK_SUCCESS) {
        printf("Failed to begin command buffer.\n");
        return;
    }

    vkCmdBeginRenderPass(cmd, &rp_offscreen, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdSetViewport(cmd, 0, 1, &vw_offscreen);
        vkCmdSetScissor(cmd, 0, 1, &sc_offscreen);
        vkCmdBindPipeline(cmd, p_bindless_offscreen_2d->bind_point, p_bindless_offscreen_2d->pipeline);
        vkCmdBindDescriptorSets(cmd, p_bindless_offscreen_2d->bind_point, p_bindless_offscreen_2d->layout, 0, 2, p_bindless_offscreen_2d->d_sets, 0, NULL);

        uint32_t instance_count = c->sprite_renderers.elem_counts;

        if (instance_count > 0) {
            vkCmdDraw(cmd, 6, instance_count, 0, 0);
        }

    vkCmdEndRenderPass(cmd);

    vkCmdBeginRenderPass(cmd, &rp_present, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdSetViewport(cmd, 0, 1, &vw_present);
        vkCmdSetScissor(cmd, 0, 1, &sc_present);
        vkCmdBindPipeline(cmd, p_post_process->bind_point, p_post_process->pipeline);
        vkCmdBindDescriptorSets(cmd, p_post_process->bind_point, p_post_process->layout, 0, 1, p_post_process->d_sets, 0, NULL);
        vkCmdPushConstants(cmd, p_post_process->layout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(RnPostProcess), &r->post_process);
        vkCmdDraw(cmd, 6, 1, 0, 0);

    vkCmdEndRenderPass(cmd);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
        printf("Failed to end command buffer.\n");
        return;
    }
}





