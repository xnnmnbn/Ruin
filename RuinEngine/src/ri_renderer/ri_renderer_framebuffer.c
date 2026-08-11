#include "cvec.h"
#include "ri_platform/ri_platform.h"
#include "ri_renderer.h"
#include <stdint.h>
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



void ri_renderer_create_framebuffer_offscreen(RI_Renderer *r) {

    VkImageCreateInfo ii = {0};
    ii.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ii.extent.width  = r->active_config.resolution_x;
    ii.extent.height = r->active_config.resolution_y;
    ii.extent.depth  = 1;
    ii.imageType = VK_IMAGE_TYPE_2D;
    ii.format    = r->swapchain.swapchain_image_format;
    ii.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ii.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
             | VK_IMAGE_USAGE_SAMPLED_BIT;
    ii.arrayLayers = 1;
    ii.mipLevels   = 1;
    ii.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ii.samples  = VK_SAMPLE_COUNT_1_BIT;
    ii.tiling   = VK_IMAGE_TILING_OPTIMAL;

    VmaAllocationCreateInfo iai = {0};
    iai.usage = VMA_MEMORY_USAGE_AUTO;

    if (vmaCreateImage(
        r->core.allocator, &ii, &iai,
        &r->framebuffers.offscreen.color_image,
        &r->framebuffers.offscreen.color_alloc,
        NULL
    ) != VK_SUCCESS) {
        printf("Failed to create Image for Bindless Offscreen Pipeline 2D.\n");
        return;
    }

    VkImageViewCreateInfo vi = {0};
    vi.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    vi.image = r->framebuffers.offscreen.color_image;
    vi.format = r->swapchain.swapchain_image_format;
    vi.viewType = VK_IMAGE_VIEW_TYPE_2D;
    vi.subresourceRange.layerCount = 1;
    vi.subresourceRange.levelCount = 1;
    vi.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    if (vkCreateImageView(r->core.device, &vi, NULL, &r->framebuffers.offscreen.color_view) != VK_SUCCESS) {
        printf("Failed to create Image View for Bindless Offscreen Pipeline 2D.\n");
        return;
    }

    
    VkFramebufferCreateInfo fi = {0};
    fi.sType        = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fi.pAttachments = &r->framebuffers.offscreen.color_view;
    fi.attachmentCount = 1;
    fi.layers = 1;
    fi.width  = r->active_config.resolution_x;
    fi.height = r->active_config.resolution_y;
    fi.renderPass = r->renderpasses.offscreen_pass;

    if (vkCreateFramebuffer(r->core.device, &fi, NULL, &r->framebuffers.offscreen.framebuffer) != VK_SUCCESS) {
        printf("Failed to create Framebuffer for Offscreen Pipeline.\n");
        return;
    }

    r->framebuffers.offscreen.extent = (VkExtent2D) {
        .width = r->active_config.resolution_x,
        .height = r->active_config.resolution_y
    };

    VkSamplerCreateInfo si = {0};
    si.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    si.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    si.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    si.minFilter = VK_FILTER_NEAREST;
    si.magFilter = VK_FILTER_NEAREST;
    si.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    si.maxLod = 0.0f;
    si.maxAnisotropy = r->active_config.max_anisotropy;

    if (vkCreateSampler(r->core.device, &si, NULL, &r->framebuffers.offscreen.color_sampler) != VK_SUCCESS) {
        printf("Failed to create Offscreen Framebuffer Sampler.\n");
        return;
    }
    
    printf("Offscreen Framebuffer created.\n");
    
}

















