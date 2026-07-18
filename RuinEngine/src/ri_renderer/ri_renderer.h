#ifndef RI_RENDERER_H
#define RI_RENDERER_H

#include "ri_platform/ri_platform.h"
#include "ruin.h"
#include <cglm/types.h>
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "vk_mem_alloc.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef RUIN_ENABLE_DEBUG
    #define enable_validation 1
#else
    #define enable_validation 0
#endif


#define MAX_SWAPCHAIN_IMAGES   8
#define MAX_FRAMES_IN_FLIGHT   8
#define MAX_DESCRIPTOR_LAYOUTS 8
#define MAX_DESCRIPTOR_SETS    3

typedef struct {
    vec3 pos;
    vec3 nor;
    vec2 uv;
} Vertex3D;


typedef struct {
    uint32_t graphics_family;
    uint32_t present_family;
} QueueFamilyIndices;

typedef struct {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR      *formats;
    VkPresentModeKHR        *present_modes;
    uint32_t                 format_count;
    uint32_t                 present_mode_count;
} SwapchainSupportDetails;

typedef struct {
    void    *code;
    uint32_t len;
} Text;


typedef struct {
    VkInstance               instance;
    VkDebugUtilsMessengerEXT messenger;
    VkSurfaceKHR             surface;
    VkPhysicalDevice         physical_device;
    VkDevice                 device;
    VkQueue                  graphics_queue;
    VkQueue                  present_queue;
    VmaAllocator             allocator;
} RI_Renderer_Core;

typedef struct {
    VkSwapchainKHR swapchain;
    VkFormat       swapchain_image_format;
    VkExtent2D     swapchain_extent;
    VkImage        swapchain_images[MAX_SWAPCHAIN_IMAGES];
    VkImageView    swapchain_image_views[MAX_SWAPCHAIN_IMAGES];
    VkFramebuffer  swapchain_framebuffers[MAX_SWAPCHAIN_IMAGES];
    uint32_t       swapchain_image_count;
} RI_Renderer_Swapchain;

typedef struct {
    VkFramebuffer framebuffer;
    VkExtent2D    extent;

    VmaAllocation color_alloc;
    VkImage       color_image;
    VkImageView   color_view;
    VkSampler     color_sampler;

    VmaAllocation depth_alloc;
    VkImage       depth_image;
    VkImageView   depth_view;
    VkSampler     depth_sampler;
} RI_Renderer_Framebuffer;

typedef struct {
    RI_Renderer_Framebuffer offscreen;
} RI_Renderer_Framebuffers;

typedef struct {
    VkRenderPass shadow_pass;
    VkRenderPass geometry_pass;
    VkRenderPass present_pass;

    VkRenderPass offscreen_pass;
} RI_Renderer_RenderPasses;

typedef struct {
    VkPipeline            pipeline;
    VkPipelineLayout      layout;
    VkPipelineBindPoint   bind_point;
    VkDescriptorSetLayout d_set_layouts[MAX_DESCRIPTOR_LAYOUTS];
    VkDescriptorSet       d_sets[MAX_DESCRIPTOR_SETS];
    uint32_t              d_set_count;
} RI_Renderer_Pipeline;

typedef struct {
    VkDescriptorPool descriptor_pool;
    RI_Renderer_Pipeline test_pipeline;
    // RI_Renderer_Pipeline bindless_pipeline_2d;

    RI_Renderer_Pipeline bindless_offscreen_2d;
    RI_Renderer_Pipeline post_process_pipeline;
} RI_Renderer_Pipelines;

typedef struct {
    VkCommandPool   command_pool;
    VkCommandBuffer command_buffers[MAX_FRAMES_IN_FLIGHT];
} RI_Renderer_Commands;

typedef struct {
    VkSemaphore image_available_semaphores[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore render_finished_semaphores[MAX_SWAPCHAIN_IMAGES];
    VkFence     in_flight_fences[MAX_FRAMES_IN_FLIGHT];
    uint32_t    current_frame;
    uint8_t     skip_wait;
} RI_Renderer_Sync;



typedef struct {
    VkBuffer          buffer;
    VmaAllocation     allocation;
    VmaAllocationInfo info;
    VkDeviceSize      size;
    void             *mapped;
} RI_Renderer_GPU_Buffer;

typedef struct {
    RI_Renderer_GPU_Buffer camera_ubo;
    RI_Renderer_GPU_Buffer entity_ssbo;
    RI_Renderer_GPU_Buffer matrix_ssbo;
    RI_Renderer_GPU_Buffer material_ssbo;
} RI_Renderer_GPU_Buffers2D;


typedef struct {
    RnConfigRenderer         active_config;
    RnConfigRenderer         pending_config;
    RnPostProcess            post_process;
    RI_Renderer_Core         core;
    RI_Renderer_Swapchain    swapchain;
    RI_Renderer_Framebuffers framebuffers;
    RI_Renderer_RenderPasses renderpasses;
    RI_Renderer_Pipelines    pipelines;
    RI_Renderer_Commands     commands;
    RI_Renderer_Sync         sync;

    RI_Renderer_GPU_Buffers2D gpu_buffers_2d;
    RI_Renderer_GPU_Buffer    gpu_buffer_post_process;
} RI_Renderer;



QueueFamilyIndices find_queue_family_indices(RI_Renderer_Core *c);
SwapchainSupportDetails query_swapchain_support(RI_Renderer *r);
VkSurfaceFormatKHR choose_swap_surface_format(VkSurfaceFormatKHR *formats, uint32_t count);
VkPresentModeKHR choose_swap_present_mode(VkPresentModeKHR *mods, uint32_t count);
VkExtent2D choose_swap_extent(VkSurfaceCapabilitiesKHR *caps, RI_Platform *p);

void create_memory_allocator(RI_Renderer *r);

Text read_file(const char *path);

VkShaderModule create_shader_module(RI_Renderer *r, unsigned char *code, unsigned int len);



void ri_renderer_create_instance(RI_Renderer *r, RI_Platform *p);
void ri_renderer_create_debug_messenger(RI_Renderer *r);
void ri_renderer_create_surface(RI_Renderer *r, RI_Platform *p);
void ri_renderer_select_physical_device(RI_Renderer *r);
void ri_renderer_create_logical_device(RI_Renderer *r);


void ri_renderer_create_vma(RI_Renderer *r);
void ri_renderer_kill_vma(RI_Renderer *r);


void ri_renderer_init(RI_Renderer *r, RI_Platform *p);

void ri_renderer_create_swapchain(RI_Renderer *r, RI_Platform *p);
void ri_renderer_get_swapchain_image_views(RI_Renderer *r);
void ri_renderer_recreate_swapchain(RI_Renderer *r, RI_Platform *p);


void ri_renderer_create_renderpass_present(RI_Renderer *r);
void ri_renderer_create_renderpass_offscreen(RI_Renderer *r);


void ri_renderer_create_swapchain_framebuffers(RI_Renderer *r, RI_Platform *p);
void ri_renderer_create_framebuffer_offscreen(RI_Renderer *r);


void ri_renderer_create_commands(RI_Renderer *r);
VkCommandBuffer ri_renderer_single_cmd_begin(RI_Renderer *r);
void            ri_renderer_single_cmd_end(RI_Renderer *r, VkCommandBuffer cmd);


void ri_renderer_create_sync_objects(RI_Renderer *r);



void ri_renderer_init_gpu_buffers_2d(RI_Renderer *r);
void ri_renderer_kill_gpu_buffers_2d(RI_Renderer *r);
void ri_renderer_init_gpu_buffer_post_process(RI_Renderer *r);
void ri_renderer_kill_gpu_buffer_post_process(RI_Renderer *r);


void ri_renderer_create_descriptor_pool(RI_Renderer *r);

void ri_renderer_create_pipeline_test(RI_Renderer *r);

// void ri_renderer_create_pipeline_bindless_2d(RI_Renderer *r);

void ri_renderer_create_pipeline_bindless_offscreen_2d(RI_Renderer *r);
void ri_renderer_create_post_process_pipeline(RI_Renderer *r);




void ri_renderer_kill(RI_Renderer *r);


#ifdef __cplusplus
}
#endif

#endif
