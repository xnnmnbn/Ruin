#include "ri_assets/ri_assets.h"
#include "ri_components/ri_components.h"
#include "ri_renderer/ri_renderer.h"
#include "ruin.h"
#include "ruin__internal.h"
#include <SDL3/SDL_events.h>
#include <cglm/cglm.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

void draw_frame(RuinInternal *engine) {
    VkDevice device = engine->renderer.core.device;

    uint32_t current_frame = engine->renderer.sync.current_frame;
    uint32_t image_index;

    //if (!r->sync.skip_wait) {
        vkWaitForFences(device, 1, &(engine->renderer.sync.in_flight_fences[current_frame]), VK_TRUE, UINT64_MAX);
    //}

    //r->sync.skip_wait = 0;

    VkResult res = vkAcquireNextImageKHR(
        device,
        engine->renderer.swapchain.swapchain,
        UINT64_MAX,
        engine->renderer.sync.image_available_semaphores[current_frame],
        VK_NULL_HANDLE,
        &image_index
    );

    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        ri_renderer_recreate_swapchain(&engine->renderer, &engine->platform);
        return;
    } else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
        printf("COULDN'T ACQUIRE NEXT IMAGE\n");
        return;
    }

    vkResetFences(device, 1, &(engine->renderer.sync.in_flight_fences[current_frame]));

    vkResetCommandBuffer(engine->renderer.commands.command_buffers[current_frame], 0);
    // record_command(r, p, r->commands.command_buffers[current_frame], image_index);


    RI_Component_Camera *cmr;
    RnEntity c_ent = engine->components.camera_in_use;
    uint8_t c_mode = engine->components.camera_mode;

    if (c_mode == 2) {
        cmr = &(from_void(engine->components.i_camera2ds, RI_Component_Camera)[c_ent]);
    } else if (c_mode == 3) {
        cmr = &(from_void(engine->components.i_camera3ds, RI_Component_Camera)[c_ent]);
    }


    // cmr = &(engine->components.i_camera3ds[c_ent]);

    // printf("ccccccccconst char * _Nonnull format, ....\n");

    memcpy(
        engine->renderer.gpu_buffers_2d.camera_ubo.mapped,
        cmr,
        sizeof(RI_Component_Camera)
    );

    // printf("aaaaaaaaa\n");

    memcpy(
        engine->renderer.gpu_buffers_2d.entity_ssbo.mapped,
        engine->components.sprite_renderers.dense_indices.data,
        sizeof(RnEntity) * engine->components.sprite_renderers.dense_indices.len
    );

    for (uint32_t i = 0; i < engine->components.sprite_renderers.dense_indices.len; i++) {
        RnEntity e = from_void(engine->components.sprite_renderers.dense_indices.data, RnEntity)[i];

        mat4 *src = &engine->components.i_transforms.world_matrices[e];
        mat4 *dst = &(from_void(engine->renderer.gpu_buffers_2d.matrix_ssbo.mapped, mat4)[e]);

        memcpy(dst, src, sizeof(mat4));
    }

    for (uint32_t i = 0; i < engine->components.sprite_renderers.dense_indices.len; i++) {
        RnEntity e     = from_void(engine->components.sprite_renderers.dense_indices.data, RnEntity)[i];
        RnMaterial2D m = from_void(engine->components.sprite_renderers.sparse_data, RnSpriteRenderer)[e].material;

        from_void(engine->renderer.gpu_buffers_2d.material_ssbo.mapped, RnMaterial2DInfo)[e] =
            from_void(engine->assets.material2Ds.data, RnMaterial2DInfo)[m];
    }


    

    record_command_2d(&engine->renderer, &engine->platform, &engine->components, engine->renderer.commands.command_buffers[current_frame], image_index);

    

    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    VkSemaphore wait_semaphore   = engine->renderer.sync.image_available_semaphores[current_frame];
    VkSemaphore signal_semaphore = engine->renderer.sync.render_finished_semaphores[image_index];

    VkSubmitInfo si = {0};
    si.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    si.pCommandBuffers      = &(engine->renderer.commands.command_buffers[current_frame]);
    si.commandBufferCount   = 1;
    si.pWaitSemaphores      = &wait_semaphore;
    si.waitSemaphoreCount   = 1;
    si.pSignalSemaphores    = &signal_semaphore;
    si.signalSemaphoreCount = 1;
    si.pWaitDstStageMask    = wait_stages;


    if (vkQueueSubmit(engine->renderer.core.graphics_queue, 1, &si, engine->renderer.sync.in_flight_fences[current_frame]) != VK_SUCCESS) {
        printf("Failed to submit command.\n");
        return;
    }

    VkPresentInfoKHR pi = {0};
    pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    pi.pSwapchains = &engine->renderer.swapchain.swapchain;
    pi.swapchainCount = 1;
    pi.pWaitSemaphores = &signal_semaphore;
    pi.waitSemaphoreCount = 1;
    pi.pImageIndices = &image_index;

    res = vkQueuePresentKHR(engine->renderer.core.present_queue, &pi);

    if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
        ri_renderer_recreate_swapchain(&engine->renderer, &engine->platform);
    }


    engine->renderer.sync.current_frame = (engine->renderer.sync.current_frame + 1) % engine->renderer.active_config.max_frames_in_flight;


    //printf("%d\n", r->active_config.max_frames_in_flight);
    //printf("Current Frame: %d\n", r->sync.current_frame);
}
