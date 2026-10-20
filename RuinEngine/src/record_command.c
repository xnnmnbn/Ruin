#include "rn_components/rn_components.h"
#include "ruin.h"
#include "rn_engine.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>

static void image_barrier(VkCommandBuffer cmd, VkImage image,
                          VkPipelineStageFlags2 src_stage, VkAccessFlags2 src_access,
                          VkPipelineStageFlags2 dst_stage, VkAccessFlags2 dst_access,
                          VkImageLayout old_layout, VkImageLayout new_layout)
{
    VkImageMemoryBarrier2 b = {};
    b.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    b.srcStageMask        = src_stage;
    b.srcAccessMask       = src_access;
    b.dstStageMask        = dst_stage;
    b.dstAccessMask       = dst_access;
    b.oldLayout           = old_layout;
    b.newLayout           = new_layout;
    b.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    b.image               = image;
    b.subresourceRange    = (VkImageSubresourceRange){ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    VkDependencyInfo dep = {};
    dep.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dep.imageMemoryBarrierCount = 1;
    dep.pImageMemoryBarriers    = &b;
    vkCmdPipelineBarrier2KHR(cmd, &dep);
}


void record_command_2d(RuinEngine *engine, VkCommandBuffer cmd, uint32_t img_idx) {
    VkCommandBufferBeginInfo cbi = {0};
    cbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    RN_Renderer_Pipeline *p_bindless_offscreen_2d = &engine->renderer.pipelines.bindless_offscreen_2d;
    RN_Renderer_Pipeline *p_post_process          = &engine->renderer.pipelines.post_process_pipeline;

    VkClearValue clear = {};
    clear.color.float32[0] = 0.75f;
    clear.color.float32[1] = 0.0f;
    clear.color.float32[2] = 0.75f;
    clear.color.float32[3] = 1.0f;

    VkImage offscreen_image = engine->renderer.render_targets.offscreen.color_image;
    VkImage swapchain_image = engine->renderer.swapchain.swapchain_images[img_idx];
    
    RnEntity c_use = engine->components.camera_in_use;

    RnCamera2D *c2 = &(from_void(engine->components.camera2ds.begin_points[0], RnCamera2D)[c_use.id]);

    uint32_t w = 0;
    uint32_t h = 0;

    if (engine->components.camera_mode == 2) {
        w = c2->width; h = c2->height;
    }
    
    VkViewport vw_offscreen = (VkViewport) {
        .x = 0, .y = 0,
        .width  = engine->renderer.config.resolution_x,
        .height = engine->renderer.config.resolution_y,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D sc_offscreen = (VkRect2D) {
        .offset = {0},
        .extent = (VkExtent2D) {
            .width  = engine->renderer.config.resolution_x,
            .height = engine->renderer.config.resolution_y
        }
    };

    VkViewport vw_present = (VkViewport) {
        .x = 0, .y = 0,
        .width  = engine->renderer.swapchain.swapchain_extent.width,
        .height = engine->renderer.swapchain.swapchain_extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D sc_present = (VkRect2D) {
        .offset = {0},
        .extent = engine->renderer.swapchain.swapchain_extent
    };

    uint32_t screen_size[2] = {
        engine->platform.config.width,
        engine->platform.config.height
    };

    VkRenderingAttachmentInfo rai_offscreen = {0};
    rai_offscreen.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    rai_offscreen.clearValue = clear;
    rai_offscreen.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    rai_offscreen.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    rai_offscreen.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    rai_offscreen.imageView = engine->renderer.render_targets.offscreen.color_view;


    VkRenderingAttachmentInfo rai_present = {0};
    rai_present.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    rai_present.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    rai_present.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    rai_present.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    rai_present.imageView = engine->renderer.swapchain.swapchain_image_views[img_idx];

    vkResetCommandBuffer(cmd, 0);

    if (vkBeginCommandBuffer(cmd, &cbi) != VK_SUCCESS) {
        printf("Failed to begin command buffer.\n");
        return;
    }

image_barrier(cmd, offscreen_image,
    VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, 0,   // last frame's sampling
    VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

image_barrier(cmd, swapchain_image,
    VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, 0, // matches the acquire semaphore wait stage
    VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);



    VkRenderingInfo ri = {0};
    ri.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    ri.colorAttachmentCount = 1;
    ri.pColorAttachments = &rai_offscreen;
    ri.layerCount = 1; 
    ri.renderArea = sc_offscreen;


    vkCmdBeginRenderingKHR(cmd, &ri);
        vkCmdSetViewport(cmd, 0, 1, &vw_offscreen);
        vkCmdSetScissor(cmd, 0, 1, &sc_offscreen);
        vkCmdBindPipeline(cmd, p_bindless_offscreen_2d->bind_point, p_bindless_offscreen_2d->pipeline);
        vkCmdBindDescriptorSets(cmd, p_bindless_offscreen_2d->bind_point, p_bindless_offscreen_2d->layout, 0, 2, p_bindless_offscreen_2d->d_sets, 0, NULL);
        vkCmdPushConstants(cmd, p_bindless_offscreen_2d->layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4) * 2, &(from_void(engine->components.camera2ds.begin_points[1], RN_Component_Camera)[engine->components.camera_in_use.id]));
        if (engine->components.renderer2ds.elem_counts != 0) {
            vkCmdDraw(cmd, 6, engine->components.renderer2ds.elem_counts, 0, 0);
        }
    vkCmdEndRenderingKHR(cmd);

image_barrier(cmd, offscreen_image,
    VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
    VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    ri.renderArea = sc_present;
    ri.pColorAttachments = &rai_present;

    vkCmdBeginRenderingKHR(cmd, &ri);
        vkCmdSetViewport(cmd, 0, 1, &vw_present);
        vkCmdSetScissor(cmd, 0, 1, &sc_present);
        vkCmdBindPipeline(cmd, p_post_process->bind_point, p_post_process->pipeline);
        vkCmdBindDescriptorSets(cmd, p_post_process->bind_point, p_post_process->layout, 0, 1, p_post_process->d_sets, 0, NULL);
        vkCmdPushConstants(cmd, p_post_process->layout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(RnPostProcess), &engine->renderer.post_process);
        vkCmdDraw(cmd, 6, 1, 0, 0);
    vkCmdEndRenderingKHR(cmd);

image_barrier(cmd, swapchain_image,
    VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
    VK_PIPELINE_STAGE_2_NONE, 0,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
        printf("Failed to end command buffer.\n");
        return;
    }
}





