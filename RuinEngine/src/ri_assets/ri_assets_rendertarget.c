#include "cvec.h"
#include "ri_assets/ri_assets.h"
#include "ri_components/ri_components.h"
#include "ri_renderer/ri_renderer.h"
#include "ruin.h"
#include <stdint.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

void ri_assets_rendertarget_create(RI_Assets *a, RI_Renderer *r, uint32_t w, uint32_t h) {
    uint32_t gpu_idx = ri_assetstorage_idx(&a->gpu_textures);
    RI_Asset_Texture_GPU *target = &(from_void(a->gpu_textures.data, RI_Asset_Texture_GPU)[gpu_idx]);

    VkImageCreateInfo ii = {0};
    ii.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ii.extent.width  = w;
    ii.extent.height = h;
    ii.extent.depth  = 1;
    ii.imageType     = VK_IMAGE_TYPE_2D;
    ii.format        = r->swapchain.swapchain_image_format;
    ii.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ii.usage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    ii.arrayLayers   = 1;
    ii.mipLevels     = 1;
    ii.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    ii.samples       = VK_SAMPLE_COUNT_1_BIT;
    ii.tiling        = VK_IMAGE_TILING_OPTIMAL;

    VmaAllocationCreateInfo iai = {0};
    iai.usage = VMA_MEMORY_USAGE_AUTO;

    if (vmaCreateImage(
        r->core.allocator, &ii, &iai,
        &target->image,
        &target->allocation,
        NULL
    ) != VK_SUCCESS) {
        printf("Failed to create Image for Render Target #%d\n", gpu_idx);
        return;
    }

    VkImageViewCreateInfo vi = {0};
    vi.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    vi.image = target->image;
    vi.format = r->swapchain.swapchain_image_format;
    vi.viewType = VK_IMAGE_VIEW_TYPE_2D;
    vi.subresourceRange.layerCount = 1;
    vi.subresourceRange.levelCount = 1;
    vi.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    if (vkCreateImageView(r->core.device, &vi, NULL, &target->view) != VK_SUCCESS) {
        printf("Failed to create Image View for Render Target #%d\n", gpu_idx);
        return;
    }
    
    VkFramebufferCreateInfo fi = {0};
    fi.sType        = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fi.pAttachments = &target->view;
    fi.attachmentCount = 1;
    fi.layers = 1;
    fi.width  = w;
    fi.height = h;
    fi.renderPass = r->renderpasses.offscreen_pass;

    if (vkCreateFramebuffer(r->core.device, &fi, NULL, &target->framebuffer) != VK_SUCCESS) {
        printf("Failed to create Framebuffer for Render Target #%d\n", gpu_idx);
        return;
    }

    VkSamplerCreateInfo si = {0};
    si.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    si.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    si.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    si.minFilter = VK_FILTER_NEAREST;
    si.magFilter = VK_FILTER_NEAREST;
    si.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    si.maxLod = 0.0f;
    si.maxAnisotropy = r->active_config.max_anisotropy;

    if (vkCreateSampler(r->core.device, &si, NULL, &target->sampler) != VK_SUCCESS) {
        printf("Failed to create Sampler for Render Target #%d\n", gpu_idx);
        return;
    }
    
    printf("RenderTarget #%d created.\n", gpu_idx);
    ri_assetstorage_update(&a->gpu_textures);
}

void ri_assets_rendertarget_kill(RI_Assets *a, RI_Renderer *r) {
    
}








