#include "cvec.h"
#include "rn_renderer.h"
#include "ruin.h"
#include <cglm/types.h>
#include <iso646.h>
#include <stdint.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>


void rn_renderer_init_gpu_buffers_2d(RN_Renderer *r) {
    VkBufferCreateInfo camera_ubo = {0};
    camera_ubo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    camera_ubo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    camera_ubo.size  = sizeof(mat4) * 2;
    camera_ubo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo ai = {0};
    ai.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    ai.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
             | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    constexpr uint8_t gpu_buffers_2d_count = 3;
    
    VkBufferCreateInfo ssbos[gpu_buffers_2d_count] = {0};
    for (uint8_t i = 0; i < gpu_buffers_2d_count; i++) {
        ssbos[i].sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        ssbos[i].usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        ssbos[i].sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    ssbos[0].size = sizeof(RnEntity) * RUIN_MAX_ENTITIES;
    ssbos[1].size = sizeof(mat4) * RUIN_MAX_ENTITIES;
    ssbos[2].size = sizeof(RnRenderer2D) * RUIN_MAX_ENTITIES;

    if (vmaCreateBuffer(r->core.allocator, &ssbos[0], &ai, &r->gpu_buffers_2d.entity_ssbo.buffer, &r->gpu_buffers_2d.entity_ssbo.allocation, &r->gpu_buffers_2d.entity_ssbo.info) != VK_SUCCESS) goto error;
    if (vmaCreateBuffer(r->core.allocator, &ssbos[1], &ai, &r->gpu_buffers_2d.matrix_ssbo.buffer, &r->gpu_buffers_2d.matrix_ssbo.allocation, &r->gpu_buffers_2d.matrix_ssbo.info) != VK_SUCCESS) goto error;
    if (vmaCreateBuffer(r->core.allocator, &ssbos[2], &ai, &r->gpu_buffers_2d.renderer_ssbo.buffer, &r->gpu_buffers_2d.renderer_ssbo.allocation, &r->gpu_buffers_2d.renderer_ssbo.info) != VK_SUCCESS) goto error;

    RUIN_DEBUG("GPU Buffers for Bindless Pipeline 2D created.\n");

    r->gpu_buffers_2d.entity_ssbo.mapped   = r->gpu_buffers_2d.entity_ssbo.info.pMappedData;
    r->gpu_buffers_2d.matrix_ssbo.mapped   = r->gpu_buffers_2d.matrix_ssbo.info.pMappedData;
    r->gpu_buffers_2d.renderer_ssbo.mapped = r->gpu_buffers_2d.renderer_ssbo.info.pMappedData;

    VkDescriptorBufferInfo ssbo_infos[3] = {
        (VkDescriptorBufferInfo){ .buffer = r->gpu_buffers_2d.entity_ssbo.buffer,   .offset = 0, .range = VK_WHOLE_SIZE },
        (VkDescriptorBufferInfo){ .buffer = r->gpu_buffers_2d.matrix_ssbo.buffer,   .offset = 0, .range = VK_WHOLE_SIZE },
        (VkDescriptorBufferInfo){ .buffer = r->gpu_buffers_2d.renderer_ssbo.buffer, .offset = 0, .range = VK_WHOLE_SIZE }
    };

    
    VkWriteDescriptorSet writes[gpu_buffers_2d_count] = {0};

    for (uint32_t i = 0; i < gpu_buffers_2d_count; i++) {
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].dstSet = r->pipelines.bindless_offscreen_2d.d_sets[0];
        writes[i].dstBinding = i;
        writes[i].dstArrayElement = 0;
        writes[i].descriptorCount = 1;
    }

    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[0].pBufferInfo = &(ssbo_infos[0]);

    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[1].pBufferInfo = &(ssbo_infos[1]);

    writes[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[2].pBufferInfo = &(ssbo_infos[2]);

    vkUpdateDescriptorSets(r->core.device, gpu_buffers_2d_count, writes, 0, NULL);

    return;

error:
    RUIN_DEBUG("Failed to create one of the SSBOs. Cleaning up...\n");
    rn_renderer_kill_gpu_buffers_2d(r);
}

void rn_renderer_kill_gpu_buffers_2d(RN_Renderer *r) {
    if (r->gpu_buffers_2d.entity_ssbo.buffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(r->core.allocator, r->gpu_buffers_2d.entity_ssbo.buffer, r->gpu_buffers_2d.entity_ssbo.allocation);
        r->gpu_buffers_2d.entity_ssbo.buffer = VK_NULL_HANDLE;
    }
    if (r->gpu_buffers_2d.matrix_ssbo.buffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(r->core.allocator, r->gpu_buffers_2d.matrix_ssbo.buffer, r->gpu_buffers_2d.matrix_ssbo.allocation);
        r->gpu_buffers_2d.matrix_ssbo.buffer = VK_NULL_HANDLE;
    }
    if (r->gpu_buffers_2d.renderer_ssbo.buffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(r->core.allocator, r->gpu_buffers_2d.renderer_ssbo.buffer, r->gpu_buffers_2d.renderer_ssbo.allocation);
        r->gpu_buffers_2d.renderer_ssbo.buffer = VK_NULL_HANDLE;
    }
}










/*
void rn_renderer_init_gpu_buffers_gui_rect(RN_Renderer *r) {


    enum {
        buffer_count = 3
    };
    

    VmaAllocationCreateInfo ai = {0};
    ai.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    ai.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
             | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkBufferCreateInfo ssbos[buffer_count] = {0};
    for (uint8_t i = 0; i < buffer_count; i++) {
        ssbos[i].sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        ssbos[i].usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        ssbos[i].sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    ssbos[0].size = sizeof(RnGuiRectInfo) * RUIN_MAX_GUI_RECTS;
    ssbos[1].size = sizeof(RnGuiRect) * RUIN_MAX_GUI_RECTS;
    ssbos[2].size = sizeof(RnMaterial2DInfo) * RUIN_MAX_MATERIALS;

    if (vmaCreateBuffer(r->core.allocator, &ssbos[0], &ai, &r->gpu_buffers_gui_rect.rect_ssbo.buffer, &r->gpu_buffers_gui_rect.rect_ssbo.allocation, &r->gpu_buffers_gui_rect.rect_ssbo.info) != VK_SUCCESS) goto error;
    if (vmaCreateBuffer(r->core.allocator, &ssbos[1], &ai, &r->gpu_buffers_gui_rect.index_ssbo.buffer, &r->gpu_buffers_gui_rect.index_ssbo.allocation, &r->gpu_buffers_gui_rect.index_ssbo.info) != VK_SUCCESS) goto error;
    if (vmaCreateBuffer(r->core.allocator, &ssbos[2], &ai, &r->gpu_buffers_gui_rect.material_ssbo.buffer, &r->gpu_buffers_gui_rect.material_ssbo.allocation, &r->gpu_buffers_gui_rect.material_ssbo.info) != VK_SUCCESS) goto error;

    RUIN_DEBUG("GPU Buffers for GUI Rect Pipeline created.\n");

    
    r->gpu_buffers_gui_rect.rect_ssbo.mapped     = r->gpu_buffers_gui_rect.rect_ssbo.info.pMappedData;
    r->gpu_buffers_gui_rect.index_ssbo.mapped    = r->gpu_buffers_gui_rect.index_ssbo.info.pMappedData;
    r->gpu_buffers_gui_rect.material_ssbo.mapped = r->gpu_buffers_gui_rect.material_ssbo.info.pMappedData;

    

    VkDescriptorBufferInfo ssbo_infos[buffer_count] = {
        (VkDescriptorBufferInfo){ .buffer = r->gpu_buffers_gui_rect.rect_ssbo.buffer,     .offset = 0, .range = VK_WHOLE_SIZE },
        (VkDescriptorBufferInfo){ .buffer = r->gpu_buffers_gui_rect.index_ssbo.buffer,    .offset = 0, .range = VK_WHOLE_SIZE },
        (VkDescriptorBufferInfo){ .buffer = r->gpu_buffers_gui_rect.material_ssbo.buffer, .offset = 0, .range = VK_WHOLE_SIZE }
    };

    
    VkWriteDescriptorSet writes[buffer_count] = {0};

    for (uint32_t i = 0; i < buffer_count; i++) {
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].dstSet = r->pipelines.gui_rect_pipeline.d_sets[0];
        writes[i].dstBinding = i;
        writes[i].dstArrayElement = 0;
        writes[i].descriptorCount = 1;
    }

    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[0].pBufferInfo = &(ssbo_infos[0]);

    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[1].pBufferInfo = &(ssbo_infos[1]);

    writes[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[2].pBufferInfo = &(ssbo_infos[2]);

    vkUpdateDescriptorSets(r->core.device, buffer_count, writes, 0, NULL);

    return;

error:
    RUIN_DEBUG("Failed to create one of the SSBOs. Cleaning up...\n");
    rn_renderer_kill_gpu_buffers_gui_rect(r);
}

void rn_renderer_kill_gpu_buffers_gui_rect(RN_Renderer *r) {
    if (r->gpu_buffers_gui_rect.rect_ssbo.buffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(r->core.allocator, r->gpu_buffers_gui_rect.rect_ssbo.buffer, r->gpu_buffers_gui_rect.rect_ssbo.allocation);
        r->gpu_buffers_gui_rect.rect_ssbo.buffer = VK_NULL_HANDLE;
    }
    if (r->gpu_buffers_gui_rect.index_ssbo.buffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(r->core.allocator, r->gpu_buffers_gui_rect.index_ssbo.buffer, r->gpu_buffers_gui_rect.index_ssbo.allocation);
        r->gpu_buffers_gui_rect.index_ssbo.buffer = VK_NULL_HANDLE;
    }
    if (r->gpu_buffers_gui_rect.material_ssbo.buffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(r->core.allocator, r->gpu_buffers_gui_rect.material_ssbo.buffer, r->gpu_buffers_gui_rect.material_ssbo.allocation);
        r->gpu_buffers_gui_rect.material_ssbo.buffer = VK_NULL_HANDLE;
    }
}
*/

