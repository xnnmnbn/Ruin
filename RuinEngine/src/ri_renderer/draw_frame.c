#include "ri_platform/ri_platform.h"
#include "ri_renderer.h"
#include <stdint.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>

void draw_frame(RI_Renderer *r, RI_Platform *p) {
    VkDevice device = r->core.device;

    uint32_t current_frame = r->sync.current_frame;
    uint32_t image_index;

    //if (!r->sync.skip_wait) {
        vkWaitForFences(device, 1, &(r->sync.in_flight_fences[current_frame]), VK_TRUE, UINT64_MAX);
    //}

    //r->sync.skip_wait = 0;

    VkResult res = vkAcquireNextImageKHR(
        device,
        r->swapchain.swapchain,
        UINT64_MAX,
        r->sync.image_available_semaphores[current_frame],
        VK_NULL_HANDLE,
        &image_index
    );

    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        ri_renderer_recreate_swapchain(r, p);
        return;
    } else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
        printf("COULDN'T ACQUIRE NEXT IMAGE\n");
        return;
    }

    vkResetFences(device, 1, &(r->sync.in_flight_fences[current_frame]));

    vkResetCommandBuffer(r->commands.command_buffers[current_frame], 0);
    record_command(r, r->commands.command_buffers[current_frame], image_index);

    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    VkSemaphore wait_semaphore   = r->sync.image_available_semaphores[current_frame];
    VkSemaphore signal_semaphore = r->sync.render_finished_semaphores[image_index];

    VkSubmitInfo si = {0};
    si.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    si.pCommandBuffers      = &(r->commands.command_buffers[current_frame]);
    si.commandBufferCount   = 1;
    si.pWaitSemaphores      = &wait_semaphore;
    si.waitSemaphoreCount   = 1;
    si.pSignalSemaphores    = &signal_semaphore;
    si.signalSemaphoreCount = 1;
    si.pWaitDstStageMask    = wait_stages;


    if (vkQueueSubmit(r->core.graphics_queue, 1, &si, r->sync.in_flight_fences[current_frame]) != VK_SUCCESS) {
        printf("Failed to submit command.\n");
        return;
    }

    VkPresentInfoKHR pi = {0};
    pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    pi.pSwapchains = &r->swapchain.swapchain;
    pi.swapchainCount = 1;
    pi.pWaitSemaphores = &signal_semaphore;
    pi.waitSemaphoreCount = 1;
    pi.pImageIndices = &image_index;

    res = vkQueuePresentKHR(r->core.present_queue, &pi);

    if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
        ri_renderer_recreate_swapchain(r, p);
    }


    r->sync.current_frame = (r->sync.current_frame + 1) % r->active_config.max_frames_in_flight;


    //printf("%d\n", r->active_config.max_frames_in_flight);
    //printf("Current Frame: %d\n", r->sync.current_frame);
}
