#include "ri_platform/ri_platform.h"
#include "ri_renderer.h"
#include <stdlib.h>
#include <stdio.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

void ri_renderer_create_swapchain_framebuffers(RI_Renderer *r, RI_Platform *p) {
    
    for (size_t i = 0; i < r->swapchain.swapchain_image_count; i++) {

        VkImageView attachments[] = {
            r->swapchain.swapchain_image_views[i]
        };
    
        VkFramebufferCreateInfo fi = {0};
        fi.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fi.pAttachments    = attachments;
        fi.attachmentCount = 1;
        fi.layers          = 1;
        fi.width           = r->swapchain.swapchain_extent.width;
        fi.height          = r->swapchain.swapchain_extent.height;
        fi.renderPass      = r->renderpasses.present_pass;

        if (vkCreateFramebuffer(r->core.device, &fi, NULL, &(r->swapchain.swapchain_framebuffers[i])) != VK_SUCCESS) {
            printf("Failed to create swapchain framebuffer #%zu\n", i);
            return;
        }
    }

    printf("Swapchain framebuffers created.\n");
}
