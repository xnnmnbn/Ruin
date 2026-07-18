#include "cvec.h"
#include "ri_assets/ri_assets.h"
#include "ri_components/ri_components.h"
#include "ri_renderer/ri_renderer.h"
#include "ruin.h"
#include "stb_image.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>


void ri_assets_texture_init(RI_Assets *a) {
    a->cpu_textures.data = malloc(RUIN_MAX_TEXTURES * sizeof(RI_Asset_Texture_CPU));
    a->gpu_textures.data = malloc(RUIN_MAX_TEXTURES * sizeof(RI_Asset_Texture_GPU));
    a->cpu_textures.next_idx = 1;
    a->gpu_textures.next_idx = 1;

    cvec_init(a->cpu_textures.valid_indices);
    cvec_init(a->gpu_textures.valid_indices);
}

void ri_assets_texture_kill(RI_Assets *a, RI_Renderer *r) {

    RI_Asset_Texture_CPU *cpu_data = a->cpu_textures.data;
    RI_Asset_Texture_GPU *gpu_data = a->gpu_textures.data;

    for (uint32_t i = 0; i < a->gpu_textures.valid_indices.len; i++) {
        RnTexture t = from_void(a->gpu_textures.valid_indices.data, RnTexture)[i];
        vmaDestroyImage(
            r->core.allocator,
            gpu_data[t].image,
            gpu_data[t].allocation
        );

        vkDestroyImageView(r->core.device, gpu_data[t].view, NULL);
        vkDestroySampler(r->core.device, gpu_data[t].sampler, NULL);
    }

    free(a->cpu_textures.data);
    free(a->gpu_textures.data);

    cvec_kill(a->cpu_textures.valid_indices);
    cvec_kill(a->gpu_textures.valid_indices);
}

void ri_assets_texture_create_gpu_data(RI_Assets *a, RI_Renderer *r) {

    cvec_kill(a->gpu_textures.valid_indices);
    cvec_init(a->gpu_textures.valid_indices);

  
    for (uint32_t i = 0; i < a->cpu_textures.valid_indices.len; i++) {
        RnTexture *texs = a->cpu_textures.valid_indices.data;
        RI_Asset_Texture_CPU *cpu_texs = a->cpu_textures.data;
        RI_Asset_Texture_GPU *gpu_texs = a->gpu_textures.data;


        RnTexture t = texs[i];

        gpu_texs[t].width  = cpu_texs[t].width;
        gpu_texs[t].height = cpu_texs[t].height;
        gpu_texs[t].format = cpu_texs[t].format;

        VkDeviceSize image_size = cpu_texs[t].width * cpu_texs[t].height * cpu_texs[t].channels;

        VkBuffer          s_buffer;
        VmaAllocation     s_alloc;
        VmaAllocationInfo s_info;

        VkBufferCreateInfo s_bi = {0};
        s_bi.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        s_bi.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        s_bi.size  = image_size;

        VmaAllocationCreateInfo s_ai = {0};
        s_ai.usage = VMA_MEMORY_USAGE_AUTO;
        s_ai.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
                   | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        if (vmaCreateBuffer(
            r->core.allocator, &s_bi, &s_ai,
            &s_buffer, &s_alloc, &s_info
        ) != VK_SUCCESS) {
            printf("Failed to create Staging Buffer for Texture #%d.\n", t);
            return;
        }

        memcpy(s_info.pMappedData, cpu_texs[t].pixels, image_size);

        stbi_image_free(cpu_texs[t].pixels);


        VkImageCreateInfo ii = {0};
        ii.sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        ii.usage     = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        ii.imageType = VK_IMAGE_TYPE_2D;
        ii.format        = gpu_texs[t].format;
        ii.extent.width  = gpu_texs[t].width;
        ii.extent.height = gpu_texs[t].height;
        ii.extent.depth  = 1;
        ii.mipLevels     = 1;
        ii.arrayLayers   = 1;
        ii.tiling        = VK_IMAGE_TILING_OPTIMAL;
        ii.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ii.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
        ii.samples       = VK_SAMPLE_COUNT_1_BIT;

        VmaAllocationCreateInfo ai = {0};
        ai.usage = VMA_MEMORY_USAGE_AUTO;

        if(vmaCreateImage(
            r->core.allocator, &ii, &ai,
            &gpu_texs[t].image, &gpu_texs[t].allocation,
            NULL
        ) != VK_SUCCESS) {
            printf("Failed to create Image for Texture #%d.\n", t);
            vmaDestroyBuffer(r->core.allocator, s_buffer, s_alloc);
            return;
        }


        VkCommandBuffer cmd = ri_renderer_single_cmd_begin(r);
        
        VkImageMemoryBarrier b = {0};
        b.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        b.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        b.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        b.srcAccessMask = 0;
        b.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        b.image         = gpu_texs[t].image;
        b.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        b.subresourceRange.levelCount = 1;
        b.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(cmd,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, NULL, 0, NULL, 1, &b
        );

        VkBufferImageCopy region = {0};
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.layerCount = 1;
        region.imageExtent.width  = gpu_texs[t].width;
        region.imageExtent.height = gpu_texs[t].height;
        region.imageExtent.depth  = 1;
        
        vkCmdCopyBufferToImage(cmd, s_buffer, gpu_texs[t].image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        b.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        b.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        b.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        b.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(cmd,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0, 0, NULL, 0, NULL, 1, &b
        );

        ri_renderer_single_cmd_end(r, cmd);

        gpu_texs[t].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        vmaDestroyBuffer(r->core.allocator, s_buffer, s_alloc);



        VkImageViewCreateInfo vi = {0};
        vi.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        vi.viewType = VK_IMAGE_VIEW_TYPE_2D;
        vi.image = gpu_texs[t].image;
        vi.format = gpu_texs[t].format;
        vi.subresourceRange.layerCount = 1;
        vi.subresourceRange.levelCount = 1;
        vi.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        if (vkCreateImageView(r->core.device, &vi, NULL, &gpu_texs[t].view) != VK_SUCCESS) {
            printf("Failed to create Image View for Texture #%d\n", t);
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

        if (vkCreateSampler(r->core.device, &si, NULL, &gpu_texs[t].sampler) != VK_SUCCESS) {
            printf("Failed to create sampler for Texture #%d\n", t);
            return;
        }

        cvec_push(a->gpu_textures.valid_indices, t, RnTexture);
        printf("GPU Resource for Texture #%d created.\n", t);
    }
}





