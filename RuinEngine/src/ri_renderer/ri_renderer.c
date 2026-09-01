#include "ri_renderer.h"
#include "ri_platform/ri_platform.h"
#include "ruin.h"
#include <corecrt_wconio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

QueueFamilyIndices find_queue_family_indices(RI_Renderer_Core *c) {
    QueueFamilyIndices indices;
    indices.graphics_family = (uint32_t)-1;
    indices.present_family  = (uint32_t)-1;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(c->physical_device, &queue_family_count, NULL);
    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(c->physical_device, &queue_family_count, queue_families);

    for (uint32_t i = 0; i < queue_family_count; i++) {

        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
        }

        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(c->physical_device, i, c->surface, &present_support);

        if (present_support) {
            indices.present_family = i;
        }

        if ((indices.graphics_family != (uint32_t)-1) && (indices.present_family != (uint32_t)-1)) {
            break;
        }

    }

    return indices;
}



SwapchainSupportDetails query_swapchain_support(RI_Renderer *r) {
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(r->core.physical_device, r->core.surface, &details.capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(r->core.physical_device, r->core.surface, &details.format_count, NULL);
    if (details.format_count != 0) {
        details.formats = malloc(details.format_count * sizeof(VkSurfaceFormatKHR));
        vkGetPhysicalDeviceSurfaceFormatsKHR(r->core.physical_device, r->core.surface, &details.format_count, details.formats);
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(r->core.physical_device, r->core.surface, &details.present_mode_count, NULL);
    if (details.present_mode_count != 0) {
        details.present_modes = malloc(details.present_mode_count * sizeof(VkPresentModeKHR));
        vkGetPhysicalDeviceSurfacePresentModesKHR(r->core.physical_device, r->core.surface, &details.present_mode_count, details.present_modes);
    }


    if (details.format_count == 0) {
        printf("No surface format found.\n");
        exit(1);
    }

    if (details.present_mode_count == 0) {
        printf("No present mode found.\n");
        exit(1);
    }

    return details;
}



Text read_file(const char *path) {
    Text txt = {0};

    FILE *f = fopen(path, "rb");

    if (!f) {
        printf("Failed to open file %s\n", path);
        return txt;
    }
    
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    rewind(f);

    txt.code = malloc(sizeof(char) * len);
    txt.len  = len;
    fread((void*)txt.code, sizeof(char), len, f);

    fclose(f);

    return txt;
}


void ri_renderer_init(RI_Renderer *r, RI_Platform *p) {
    
    ri_renderer_create_instance(r, p);
    if (enable_validation) {
        ri_renderer_create_debug_messenger(r);
    }
    ri_renderer_create_surface(r, p);
    ri_renderer_select_physical_device(r);
    ri_renderer_create_logical_device(r);

    ri_renderer_create_vma(r);

    ri_renderer_create_swapchain(r, p);
    ri_renderer_get_swapchain_image_views(r);


    ri_renderer_create_renderpass_offscreen(r);
    ri_renderer_create_renderpass_present(r);

    ri_renderer_create_framebuffer_offscreen(r);
    ri_renderer_create_framebuffer_post_process(r, p);

    ri_renderer_create_descriptor_pool(r);

    ri_renderer_create_pipeline_bindless_offscreen_2d(r);
    ri_renderer_create_pipeline_post_process(r);
    ri_renderer_create_pipeline_rect_gui(r);
    
    ri_renderer_create_swapchain_framebuffers(r, p);
    ri_renderer_create_commands(r);
    ri_renderer_create_sync_objects(r);

    ri_renderer_init_gpu_buffers_2d(r);
    ri_renderer_init_gpu_buffers_gui_rect(r);

    r->post_process.brightness = 1.0f;
    r->post_process.saturation = 1.0f;
    r->post_process.contrast = 1.0f;
    r->post_process.invert = 0.0f;
    r->post_process.tint = (RnColor){ 1.0f, 1.0f, 1.0f, 1.0f };
}



void ri_renderer_kill(RI_Renderer *r) {
    
    if (r->core.device == VK_NULL_HANDLE) {
        printf("Renderer was not killed since it has never been created.\n");
        return;
    }
    vkDeviceWaitIdle(r->core.device);

    ri_renderer_kill_gpu_buffers_gui_rect(r);
    ri_renderer_kill_gpu_buffers_2d(r);

    vmaDestroyImage(r->core.allocator, r->framebuffers.offscreen.color_image, r->framebuffers.offscreen.color_alloc);
    vkDestroyImageView(r->core.device, r->framebuffers.offscreen.color_view, NULL);
    vkDestroySampler(r->core.device, r->framebuffers.offscreen.color_sampler, NULL);
    vkDestroyFramebuffer(r->core.device, r->framebuffers.offscreen.framebuffer, NULL);

    
    vmaDestroyImage(r->core.allocator, r->framebuffers.post_process.color_image, r->framebuffers.post_process.color_alloc);
    vkDestroyImageView(r->core.device, r->framebuffers.post_process.color_view, NULL);
    vkDestroySampler(r->core.device, r->framebuffers.post_process.color_sampler, NULL);
    vkDestroyFramebuffer(r->core.device, r->framebuffers.post_process.framebuffer, NULL);


    
    ri_renderer_kill_vma(r);
    
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(r->core.device, r->sync.image_available_semaphores[i], NULL);
        vkDestroyFence(r->core.device, r->sync.in_flight_fences[i], NULL);
    }
    for (size_t i = 0; i < r->swapchain.swapchain_image_count; i++) {
        vkDestroySemaphore(r->core.device, r->sync.render_finished_semaphores[i], NULL);
    }
    vkFreeCommandBuffers(r->core.device, r->commands.command_pool, MAX_FRAMES_IN_FLIGHT, r->commands.command_buffers);
    vkDestroyCommandPool(r->core.device, r->commands.command_pool, NULL);
    for (size_t i = 0; i < r->swapchain.swapchain_image_count; i++) {
        vkDestroyFramebuffer(r->core.device, r->swapchain.swapchain_framebuffers[i], NULL);
    }




    vkDestroyDescriptorPool(r->core.device, r->pipelines.descriptor_pool, NULL);

    vkDestroyPipeline(r->core.device, r->pipelines.bindless_offscreen_2d.pipeline, NULL);
    vkDestroyPipelineLayout(r->core.device, r->pipelines.bindless_offscreen_2d.layout, NULL);
    for (size_t i = 0; i < r->pipelines.bindless_offscreen_2d.d_set_count; i++) {
        vkDestroyDescriptorSetLayout(r->core.device, r->pipelines.bindless_offscreen_2d.d_set_layouts[i], NULL);
    }
    
    vkDestroyPipeline(r->core.device, r->pipelines.post_process_pipeline.pipeline, NULL);
    vkDestroyPipelineLayout(r->core.device, r->pipelines.post_process_pipeline.layout, NULL);
    for (size_t i = 0; i < r->pipelines.post_process_pipeline.d_set_count; i++) {
        vkDestroyDescriptorSetLayout(r->core.device, r->pipelines.post_process_pipeline.d_set_layouts[i], NULL);
    }

    vkDestroyPipeline(r->core.device, r->pipelines.gui_rect_pipeline.pipeline, NULL);
    vkDestroyPipelineLayout(r->core.device, r->pipelines.gui_rect_pipeline.layout, NULL);
    for (size_t i = 0; i < r->pipelines.gui_rect_pipeline.d_set_count; i++) {
        vkDestroyDescriptorSetLayout(r->core.device, r->pipelines.gui_rect_pipeline.d_set_layouts[i], NULL);
    }    




    vkDestroyRenderPass(r->core.device, r->renderpasses.offscreen_pass, NULL);
    vkDestroyRenderPass(r->core.device, r->renderpasses.present_pass, NULL);



    
    for (size_t i = 0; i < r->swapchain.swapchain_image_count; i++) {
        vkDestroyImageView(r->core.device, r->swapchain.swapchain_image_views[i], NULL);
    }
    vkDestroySwapchainKHR(r->core.device, r->swapchain.swapchain, NULL);
 
    
    vkDestroyDevice(r->core.device, NULL);

    if (enable_validation) {
        
    }
    vkDestroySurfaceKHR(r->core.instance, r->core.surface, NULL);
    vkDestroyInstance(r->core.instance, NULL);

    printf("Renderer killed.\n");
}





