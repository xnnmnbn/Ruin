#include "rn_renderer.h"
#include "ruin.h"
#include <stddef.h>
#include <stdio.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

void rn_renderer_create_sync_objects(RN_Renderer *r) {

    for (size_t i = 0; i < r->swapchain.swapchain_image_count; i++) {
        VkSemaphoreCreateInfo si = {0};
        si.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        if (vkCreateSemaphore(r->core.device, &si, NULL, &(r->sync.render_finished_semaphores[i])) != VK_SUCCESS) {
            RUIN_DEBUG("Failed to create Render Finished Semaphore #%zu\n", i);
            return;
        }
    }

  
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkSemaphoreCreateInfo si = {0};
        si.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fi = {0};
        fi.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fi.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateSemaphore(r->core.device, &si, NULL, &(r->sync.image_available_semaphores[i])) != VK_SUCCESS) {
            RUIN_DEBUG("Failed to craete Image Available Semaphore #%zu\n", i);
            return;
        }

        if (vkCreateFence(r->core.device, &fi, NULL, &(r->sync.in_flight_fences[i])) != VK_SUCCESS) {
            RUIN_DEBUG("Failed to create In Flight Fence #%zu\n", i);
            return;
        }
    }

    r->sync.current_frame = 0;

    RUIN_DEBUG("Sync Objects created.\n");
}






