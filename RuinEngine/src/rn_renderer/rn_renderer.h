#ifndef RI_RENDERER_H
#define RI_RENDERER_H

#include "rn_platform/rn_platform.h"
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

#ifndef NDEBUG
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
} RN_Renderer_Core;

typedef struct {
    VkSwapchainKHR swapchain;
    VkFormat       swapchain_image_format;
    VkExtent2D     swapchain_extent;
    VkImage        swapchain_images[MAX_SWAPCHAIN_IMAGES];
    VkImageView    swapchain_image_views[MAX_SWAPCHAIN_IMAGES];
    VkFramebuffer  swapchain_framebuffers[MAX_SWAPCHAIN_IMAGES];
    uint32_t       swapchain_image_count;
} RN_Renderer_Swapchain;

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

    uint32_t gpu_index;
} RN_Renderer_Framebuffer;

typedef struct {
    RN_Renderer_Framebuffer offscreen;
    RN_Renderer_Framebuffer post_process;
} RN_Renderer_Framebuffers;

typedef struct {
    VkRenderPass shadow_pass;
    VkRenderPass geometry_pass;
    VkRenderPass offscreen_pass;

    VkRenderPass present_pass;
} RN_Renderer_RenderPasses;

typedef struct {
    VkPipeline            pipeline;
    VkPipelineLayout      layout;
    VkPipelineBindPoint   bind_point;
    VkDescriptorSetLayout d_set_layouts[MAX_DESCRIPTOR_LAYOUTS];
    VkDescriptorSet       d_sets[MAX_DESCRIPTOR_SETS];
    uint32_t              d_set_count;
} RN_Renderer_Pipeline;

typedef struct {
    VkDescriptorPool descriptor_pool;
    // RN_Renderer_Pipeline test_pipeline;

    RN_Renderer_Pipeline bindless_offscreen_2d;
    RN_Renderer_Pipeline post_process_pipeline;
    // RN_Renderer_Pipeline gui_rect_pipeline;
} RN_Renderer_Pipelines;

typedef struct {
    VkCommandPool   command_pool;
    VkCommandBuffer command_buffers[MAX_FRAMES_IN_FLIGHT];
} RN_Renderer_Commands;

typedef struct {
    VkSemaphore image_available_semaphores[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore render_finished_semaphores[MAX_SWAPCHAIN_IMAGES];
    VkFence     in_flight_fences[MAX_FRAMES_IN_FLIGHT];
    uint32_t    current_frame;
} RN_Renderer_Sync;



typedef struct {
    VkBuffer          buffer;
    VmaAllocation     allocation;
    VmaAllocationInfo info;
    VkDeviceSize      size;
    void             *mapped;
} RN_Renderer_GPU_Buffer;

typedef struct {
    RN_Renderer_GPU_Buffer entity_ssbo;
    RN_Renderer_GPU_Buffer matrix_ssbo;
    RN_Renderer_GPU_Buffer renderer_ssbo;
} RN_Renderer_GPU_Buffers_Bindless_2D;

/*
typedef struct {
    RN_Renderer_GPU_Buffer rect_ssbo;
    RN_Renderer_GPU_Buffer index_ssbo;
    RN_Renderer_GPU_Buffer material_ssbo;
} RN_Renderer_GPU_Buffers_GuiRect;
*/

typedef struct {
    RnConfigRenderer         active_config;
    RnConfigRenderer         pending_config;
    RnPostProcess            post_process;
    RN_Renderer_Core         core;
    RN_Renderer_Swapchain    swapchain;
    RN_Renderer_Framebuffers framebuffers;
    RN_Renderer_RenderPasses renderpasses;
    RN_Renderer_Pipelines    pipelines;
    RN_Renderer_Commands     commands;
    RN_Renderer_Sync         sync;

    RN_Renderer_GPU_Buffers_Bindless_2D gpu_buffers_2d;
    // RN_Renderer_GPU_Buffers_GuiRect     gpu_buffers_gui_rect;
} RN_Renderer;



QueueFamilyIndices find_queue_family_indices(RN_Renderer_Core *c);
SwapchainSupportDetails query_swapchain_support(RN_Renderer *r);
VkSurfaceFormatKHR choose_swap_surface_format(VkSurfaceFormatKHR *formats, uint32_t count);
VkPresentModeKHR choose_swap_present_mode(VkPresentModeKHR *mods, uint32_t count);
VkExtent2D choose_swap_extent(VkSurfaceCapabilitiesKHR *caps, RN_Platform *p);

void create_memory_allocator(RN_Renderer *r);

Text read_file(const char *path);

VkShaderModule create_shader_module(RN_Renderer *r, unsigned const char *code, unsigned int len);



void rn_renderer_create_instance(RN_Renderer *r, RN_Platform *p);
void rn_renderer_create_debug_messenger(RN_Renderer *r);
void rn_renderer_create_surface(RN_Renderer *r, RN_Platform *p);
void rn_renderer_select_physical_device(RN_Renderer *r);
void rn_renderer_create_logical_device(RN_Renderer *r);


void rn_renderer_create_vma(RN_Renderer *r);
void rn_renderer_kill_vma(RN_Renderer *r);


void rn_renderer_init(RN_Renderer *r, RN_Platform *p);

void rn_renderer_create_swapchain(RN_Renderer *r, RN_Platform *p);
void rn_renderer_get_swapchain_image_views(RN_Renderer *r);
void rn_renderer_recreate_swapchain(RN_Renderer *r, RN_Platform *p);


void rn_renderer_create_renderpass_present(RN_Renderer *r);
void rn_renderer_create_renderpass_offscreen(RN_Renderer *r);


void rn_renderer_create_swapchain_framebuffers(RN_Renderer *r, RN_Platform *p);
void rn_renderer_create_framebuffer_offscreen(RN_Renderer *r);
void rn_renderer_create_framebuffer_post_process(RN_Renderer *r, RN_Platform *p);


void rn_renderer_create_commands(RN_Renderer *r);
VkCommandBuffer rn_renderer_single_cmd_begin(RN_Renderer *r);
void            rn_renderer_single_cmd_end(RN_Renderer *r, VkCommandBuffer cmd);


void rn_renderer_create_sync_objects(RN_Renderer *r);



void rn_renderer_init_gpu_buffers_2d(RN_Renderer *r);
void rn_renderer_kill_gpu_buffers_2d(RN_Renderer *r);

/*
void rn_renderer_init_gpu_buffers_gui_rect(RN_Renderer *r);
void rn_renderer_kill_gpu_buffers_gui_rect(RN_Renderer *r);
*/

void rn_renderer_create_descriptor_pool(RN_Renderer *r);

void rn_renderer_create_pipeline_test(RN_Renderer *r);

void rn_renderer_create_pipeline_bindless_offscreen_2d(RN_Renderer *r);
void rn_renderer_create_pipeline_post_process(RN_Renderer *r);

// void rn_renderer_create_pipeline_rect_gui(RN_Renderer *r);



void rn_renderer_kill(RN_Renderer *r);


#ifdef __cplusplus
}
#endif

#endif
