#include "rn_renderer/rn_renderer.h"
#include "ruin.h"
#include <vulkan/vulkan_core.h>

void rn_renderer_create_rendertarget_offscreen_2d(RN_Renderer *r) {

    auto offscreen = &r->render_targets.offscreen;
    
    offscreen->extent = (VkExtent2D) {
        .width = r->config.resolution_x,
        .height = r->config.resolution_y,
    };

    VkImageCreateInfo ii = {};
    ii.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ii.extent.width = offscreen->extent.width;
    ii.extent.height = offscreen->extent.height;
    ii.extent.depth = 1;
    ii.imageType = VK_IMAGE_TYPE_2D;
    ii.format = VK_FORMAT_R8G8B8A8_SRGB;
    ii.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ii.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
               VK_IMAGE_USAGE_SAMPLED_BIT          |
               VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    ii.samples = VK_SAMPLE_COUNT_1_BIT;
    ii.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ii.tiling = VK_IMAGE_TILING_OPTIMAL;
    ii.arrayLayers = 1;
    ii.mipLevels = 1;
    ii.pQueueFamilyIndices = &r->core.queue_families.graphics_family;
    ii.queueFamilyIndexCount = 1;

    VmaAllocationCreateInfo ai = {};
    ai.usage = VMA_MEMORY_USAGE_AUTO;

    VkResult res = vmaCreateImage(r->core.allocator, &ii, &ai, &offscreen->color_image, &offscreen->color_alloc, nullptr);

    if (res != VK_SUCCESS) {
        RUIN_DEBUG("Failed to create Offscreen Image: %i\n", res);
        return;
    }


    VkImageViewCreateInfo vi = {};
    vi.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    vi.image = offscreen->color_image;
    vi.format = VK_FORMAT_R8G8B8A8_SRGB;
    vi.viewType = VK_IMAGE_VIEW_TYPE_2D;
    vi.components.r = VK_COMPONENT_SWIZZLE_R;
    vi.components.g = VK_COMPONENT_SWIZZLE_G;
    vi.components.b = VK_COMPONENT_SWIZZLE_B;
    vi.components.a = VK_COMPONENT_SWIZZLE_A;
    vi.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    vi.subresourceRange.levelCount = 1;
    vi.subresourceRange.layerCount = 1;

    if (vkCreateImageView(r->core.device, &vi, nullptr, &offscreen->color_view) != VK_SUCCESS) {
        RUIN_DEBUG("Failed to create Offscreen Image View.\n");
        return;
    }

    VkSamplerCreateInfo si = {};
    si.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    si.anisotropyEnable = VK_TRUE;
    si.maxAnisotropy = r->config.max_anisotropy;
    si.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    si.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    si.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    si.magFilter = VK_FILTER_NEAREST;
    si.minFilter = VK_FILTER_NEAREST;

    if (vkCreateSampler(r->core.device, &si, nullptr, &offscreen->color_sampler) != VK_SUCCESS) {
        RUIN_DEBUG("Failed to create Offscreen Sampler.\n");
        return;
    }

    RUIN_DEBUG("Offscreen Render Target created.\n");
}
