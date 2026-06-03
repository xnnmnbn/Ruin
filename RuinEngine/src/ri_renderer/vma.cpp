#include <vulkan/vulkan_core.h>
#define VMA_IMPLEMENTATION
#define VMA_VULKAN_VERSION 1003000

#include "ri_renderer.h"


void ri_renderer_create_vma(RI_Renderer *r) {
    VmaAllocatorCreateInfo vma = {0};
    vma.instance = r->core.instance;
    vma.physicalDevice = r->core.physical_device;
    vma.device = r->core.device;
    vma.vulkanApiVersion = VK_API_VERSION_1_3;

    if (vmaCreateAllocator(&vma, &r->core.allocator) != VK_SUCCESS) {
        printf("Failed to create Vulkan Memory Allocator.\n");
        return;
    }

    printf("Memory Allocator created.\n");
}

void ri_renderer_kill_vma(RI_Renderer *r) {
    vmaDestroyAllocator(r->core.allocator);
}
