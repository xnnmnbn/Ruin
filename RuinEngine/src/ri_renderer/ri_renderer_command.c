#include "ri_renderer.h"
#include <stddef.h>
#include <stdio.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

void ri_renderer_create_commands(RI_Renderer *r) {
    VkCommandPoolCreateInfo pi = {0};
    pi.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pi.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pi.queueFamilyIndex = find_queue_family_indices(&r->core).graphics_family;

    if (vkCreateCommandPool(r->core.device, &pi, NULL, &r->commands.command_pool) != VK_SUCCESS) {
        printf("Failed to create command pool.\n");
        return;
    }

    printf("Command pool created.\n");


    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkCommandBufferAllocateInfo cmd = {0};
        cmd.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmd.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmd.commandPool        = r->commands.command_pool;
        cmd.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(r->core.device, &cmd, &(r->commands.command_buffers[i])) != VK_SUCCESS) {
            printf("Failed to allocate command buffer #%zu\n", i);
            return;
        }

        printf("Command buffer #%zu allocated.\n", i);
    }
}

VkCommandBuffer ri_renderer_single_cmd_begin(RI_Renderer *r) {
    VkCommandBufferAllocateInfo ci = {0};
    ci.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    ci.commandBufferCount = 1;
    ci.commandPool = r->commands.command_pool;
    ci.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VkCommandBuffer cmd;

    if (vkAllocateCommandBuffers(r->core.device, &ci, &cmd) != VK_SUCCESS) {
        printf("Failed to allocate single-use command buffer.\n");
        return VK_NULL_HANDLE;
    }

    VkCommandBufferBeginInfo bi = {0};
    bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(cmd, &bi) != VK_SUCCESS) {
        printf("Failed to begin single-use command buffer.\n");
        return VK_NULL_HANDLE;
    }

    return cmd;
}

void ri_renderer_single_cmd_end(RI_Renderer *r, VkCommandBuffer cmd) {
    if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
        printf("Failed to end single-use command buffer.\n");
        return;
    }

    VkSubmitInfo si = {0};
    si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    si.pCommandBuffers = &cmd;
    si.commandBufferCount = 1;


    if (vkQueueSubmit(r->core.graphics_queue, 1, &si, VK_NULL_HANDLE) != VK_SUCCESS) {
        printf("Failed to submit single-use command.\n");
        return;
    }

    
    vkQueueWaitIdle(r->core.graphics_queue);

    vkFreeCommandBuffers(r->core.device, r->commands.command_pool, 1, &cmd);
}





