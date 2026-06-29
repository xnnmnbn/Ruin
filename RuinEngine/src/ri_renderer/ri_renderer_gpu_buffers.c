#include "cvec.h"
#include "ri_renderer/ri_renderer.h"
#include "ruin.h"
#include <cglm/types.h>
#include <stdint.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>


void ri_renderer_init_gpu_buffers_2d(RI_Renderer *r) {
    VkBufferCreateInfo camera_ubo = {0};
    camera_ubo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    camera_ubo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    camera_ubo.size  = sizeof(mat4) * 2;
    camera_ubo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo ai = {0};
    ai.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    ai.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
             | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    if (vmaCreateBuffer(
        r->core.allocator,
        &camera_ubo, &ai,
        &r->gpu_buffers_2d.camera_ubo.buffer,
        &r->gpu_buffers_2d.camera_ubo.allocation,
        &r->gpu_buffers_2d.camera_ubo.info
    ) != VK_SUCCESS) {
        printf("Failed to create GPU buffer #0.\n");
        return;
    }




    VkBufferCreateInfo ssbos[4] = {0};
    for (uint8_t i = 0; i < 4; i++) {
        ssbos[i].sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        ssbos[i].usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        ssbos[i].sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    ssbos[0].size = sizeof(RnEntity) * RUIN_MAX_ENTITIES;
    ssbos[1].size = sizeof(mat4) * RUIN_MAX_ENTITIES;
    ssbos[2].size = sizeof(RnMaterial2DInfo) * RUIN_MAX_MATERIALS;

    if (vmaCreateBuffer(r->core.allocator, &ssbos[0], &ai, &r->gpu_buffers_2d.entity_ssbo.buffer, &r->gpu_buffers_2d.entity_ssbo.allocation, &r->gpu_buffers_2d.entity_ssbo.info) != VK_SUCCESS) goto error;
    if (vmaCreateBuffer(r->core.allocator, &ssbos[1], &ai, &r->gpu_buffers_2d.matrix_ssbo.buffer, &r->gpu_buffers_2d.matrix_ssbo.allocation, &r->gpu_buffers_2d.matrix_ssbo.info) != VK_SUCCESS) goto error;
    if (vmaCreateBuffer(r->core.allocator, &ssbos[2], &ai, &r->gpu_buffers_2d.material_ssbo.buffer, &r->gpu_buffers_2d.material_ssbo.allocation, &r->gpu_buffers_2d.material_ssbo.info) != VK_SUCCESS) goto error;

    printf("GPU Buffers for 2D created.\n");

    
    r->gpu_buffers_2d.camera_ubo.mapped    = r->gpu_buffers_2d.camera_ubo.info.pMappedData;
    r->gpu_buffers_2d.entity_ssbo.mapped   = r->gpu_buffers_2d.entity_ssbo.info.pMappedData;
    r->gpu_buffers_2d.matrix_ssbo.mapped   = r->gpu_buffers_2d.matrix_ssbo.info.pMappedData;
    r->gpu_buffers_2d.material_ssbo.mapped = r->gpu_buffers_2d.material_ssbo.info.pMappedData;


    VkDescriptorBufferInfo ubo_info = {0};
    ubo_info.buffer = r->gpu_buffers_2d.camera_ubo.buffer;
    ubo_info.offset = 0;
    ubo_info.range  = VK_WHOLE_SIZE;

    VkDescriptorBufferInfo ssbo_infos[3] = {
        (VkDescriptorBufferInfo){ .buffer = r->gpu_buffers_2d.entity_ssbo.buffer,   .offset = 0, .range = VK_WHOLE_SIZE },
        (VkDescriptorBufferInfo){ .buffer = r->gpu_buffers_2d.matrix_ssbo.buffer,   .offset = 0, .range = VK_WHOLE_SIZE },
        (VkDescriptorBufferInfo){ .buffer = r->gpu_buffers_2d.material_ssbo.buffer, .offset = 0, .range = VK_WHOLE_SIZE }
    };

    
    VkWriteDescriptorSet writes[4] = {0};

    for (uint32_t i = 0; i < 4; i++) {
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].dstSet = r->pipelines.bindless_pipeline_2d.d_sets[0];
        writes[i].dstBinding = i;
        writes[i].dstArrayElement = 0;
        writes[i].descriptorCount = 1;
    }

    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &ubo_info;

    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[1].pBufferInfo = &(ssbo_infos[0]);

    writes[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[2].pBufferInfo = &(ssbo_infos[1]);

    writes[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[3].pBufferInfo = &(ssbo_infos[2]);

    vkUpdateDescriptorSets(r->core.device, 4, writes, 0, NULL);

    return;

error:
    printf("Failed to create one of the SSBOs. Cleaning up...\n");
    ri_renderer_kill_gpu_buffers_2d(r);
}

void ri_renderer_kill_gpu_buffers_2d(RI_Renderer *r) {
    if (r->gpu_buffers_2d.camera_ubo.buffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(r->core.allocator, r->gpu_buffers_2d.camera_ubo.buffer, r->gpu_buffers_2d.camera_ubo.allocation);
        r->gpu_buffers_2d.camera_ubo.buffer = VK_NULL_HANDLE;
    }
    if (r->gpu_buffers_2d.entity_ssbo.buffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(r->core.allocator, r->gpu_buffers_2d.entity_ssbo.buffer, r->gpu_buffers_2d.entity_ssbo.allocation);
        r->gpu_buffers_2d.entity_ssbo.buffer = VK_NULL_HANDLE;
    }
    if (r->gpu_buffers_2d.matrix_ssbo.buffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(r->core.allocator, r->gpu_buffers_2d.matrix_ssbo.buffer, r->gpu_buffers_2d.matrix_ssbo.allocation);
        r->gpu_buffers_2d.matrix_ssbo.buffer = VK_NULL_HANDLE;
    }
    if (r->gpu_buffers_2d.material_ssbo.buffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(r->core.allocator, r->gpu_buffers_2d.material_ssbo.buffer, r->gpu_buffers_2d.material_ssbo.allocation);
        r->gpu_buffers_2d.material_ssbo.buffer = VK_NULL_HANDLE;
    }
}






