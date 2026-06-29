#include "ri_platform/ri_platform.h"
#include "ri_renderer.h"
#include "ruin.h"
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_video.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

uint32_t u_clamp(uint32_t v, uint32_t min, uint32_t max) {
    if (v < min) return min;
    if (v > max) return max;
    return  v;
}

VkSurfaceFormatKHR choose_swap_surface_format(VkSurfaceFormatKHR *formats, uint32_t count) {
    for (size_t i = 0; i < count; i++) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return formats[i];
        }
    }

    return  formats[0];
}


VkPresentModeKHR choose_swap_present_mode(VkPresentModeKHR *mods, uint32_t count) {
    for (size_t i = 0; i < count; i++) {
        if (mods[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return mods[i];
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D choose_swap_extent(VkSurfaceCapabilitiesKHR *caps, RI_Platform *p) {

    if (caps->currentExtent.width != (uint32_t)-1) {
        return caps->currentExtent;
    }

    VkExtent2D extent = {
        p->active_config.width,
        p->active_config.height
    };

    extent.width  = u_clamp(extent.width, caps->minImageExtent.width, caps->maxImageExtent.width);
    extent.height = u_clamp(extent.height, caps->minImageExtent.height, caps->maxImageExtent.height);

    return extent;
}


void ri_renderer_create_swapchain(RI_Renderer *r, RI_Platform *p) {
    SwapchainSupportDetails details = query_swapchain_support(r);
    QueueFamilyIndices indices      = find_queue_family_indices(&r->core);
    
    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(details.formats, details.format_count);
    VkPresentModeKHR   present_mode   = choose_swap_present_mode(details.present_modes, details.present_mode_count);
    VkExtent2D         extent         = choose_swap_extent(&details.capabilities, p);
    uint32_t           image_count    = details.capabilities.minImageCount + 1;

    if ((details.capabilities.maxImageCount > 0) && (image_count > details.capabilities.maxImageCount)) {
        image_count = details.capabilities.maxImageCount;
    }

    uint32_t q_families[2] = {
        indices.graphics_family,
        indices.present_family
    };

    VkSwapchainCreateInfoKHR si = {0};
    
    si.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    si.surface          = r->core.surface;
    si.minImageCount    = image_count;
    si.imageFormat      = surface_format.format;
    si.imageColorSpace  = surface_format.colorSpace;
    si.imageExtent      = extent;
    si.imageArrayLayers = 1;
    si.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    si.preTransform     = details.capabilities.currentTransform;
    si.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    si.presentMode      = present_mode;
    si.clipped          = VK_TRUE;
    si.oldSwapchain     = r->swapchain.swapchain;

    

    
    if (indices.graphics_family != indices.present_family) {
        si.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        si.queueFamilyIndexCount = 2;
        si.pQueueFamilyIndices   = q_families;
    } else {
        si.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    }

    if (vkCreateSwapchainKHR(r->core.device, &si, NULL, &r->swapchain.swapchain) != VK_SUCCESS) {
        printf("Failed to create swapchain.\n");
        free(details.formats);
        free(details.present_modes);
        return;
    }

    vkGetSwapchainImagesKHR(r->core.device, r->swapchain.swapchain, &r->swapchain.swapchain_image_count, NULL);
    vkGetSwapchainImagesKHR(r->core.device, r->swapchain.swapchain, &r->swapchain.swapchain_image_count, r->swapchain.swapchain_images);

    r->swapchain.swapchain_image_format = surface_format.format;
    r->swapchain.swapchain_extent       = extent;

    vkDestroySwapchainKHR(r->core.device, si.oldSwapchain, NULL);

    printf("Swapchain created.\n");
    printf("You have %d swapchain images.\n", r->swapchain.swapchain_image_count);

    free(details.formats);
    free(details.present_modes);
}

void ri_renderer_get_swapchain_image_views(RI_Renderer *r) {
    
    for (size_t i = 0; i < r->swapchain.swapchain_image_count; i++) {
        VkImageViewCreateInfo vi = {0};

        vi.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        vi.image    = r->swapchain.swapchain_images[i];
        vi.viewType = VK_IMAGE_VIEW_TYPE_2D;
        vi.format   = r->swapchain.swapchain_image_format;
        
        vi.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        vi.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        vi.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        vi.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        
        vi.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        vi.subresourceRange.baseMipLevel   = 0;
        vi.subresourceRange.levelCount     = 1;
        vi.subresourceRange.baseArrayLayer = 0;
        vi.subresourceRange.layerCount     = 1;

        if (vkCreateImageView(r->core.device, &vi, NULL, &(r->swapchain.swapchain_image_views[i])) != VK_SUCCESS) {
            printf("Failed to create swapchain image view #%llu", i);
            break;
        }
    }

    printf("Swapchain image views created.\n");
}



void ri_renderer_recreate_swapchain(RI_Renderer *r, RI_Platform *p) {

    vkDeviceWaitIdle(r->core.device);

    for (size_t i = 0; i < r->swapchain.swapchain_image_count; i++) {
        vkDestroyFramebuffer(r->core.device, r->swapchain.swapchain_framebuffers[i], NULL);
        vkDestroyImageView(r->core.device, r->swapchain.swapchain_image_views[i], NULL);
    }

    // vkDestroySwapchainKHR(r->core.device, r->swapchain.swapchain, NULL);

    
if (p->active_config.width == 0 || p->active_config.height == 0) return;
    
    ri_renderer_create_swapchain(r, p);
    ri_renderer_get_swapchain_image_views(r);
    ri_renderer_create_swapchain_framebuffers(r, p);

    printf("Swapchain recreated.\n");
}









