#ifndef RUIN_ASSETS_H
#define RUIN_ASSETS_H


#include "cvec.h"
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#include "ri_renderer/ri_renderer.h"

#include "ruin.h"
#include "vk_mem_alloc.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void    *data;
    cvec     valid_indices;
    cvec     free_indices;
} RI_AssetStorage;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    VkFormat format;
    uint8_t *pixels;
    uint32_t gpu_idx;
} RI_Asset_Texture_CPU;

typedef struct {
    VmaAllocation allocation;
    
    VkImage       image;
    VkImageView   view;
    VkSampler     sampler;
    uint32_t      width;
    uint32_t      height;
    VkFormat      format;
    VkImageLayout layout;

    VkFramebuffer framebuffer;
} RI_Asset_Texture_GPU;



typedef struct {
    RI_AssetStorage material2Ds;
    RI_AssetStorage material3Ds;

    RI_AssetStorage cpu_textures;
    RI_AssetStorage gpu_textures;
} RI_Assets;



uint32_t ri_assetstorage_idx(RI_AssetStorage *s);
void     ri_assetstorage_update(RI_AssetStorage *s);
void ri_assetstorage_kill(RI_AssetStorage *s, uint32_t idx);




void ri_assets_init(RI_Assets *a);
void ri_assets_kill(RI_Assets *a, RI_Renderer *r);

void ri_assets_material_init(RI_Assets *a);
void ri_assets_material_kill(RI_Assets *a);

void ri_assets_texture_init(RI_Assets *a);
void ri_assets_texture_kill(RI_Assets *a, RI_Renderer *r);

void ri_assets_rendertarget_create(RI_Assets *a, RI_Renderer *r, uint32_t w, uint32_t h);
void ri_assets_rendertarget_kill(RI_Assets *a, RI_Renderer *r);

void ri_assets_texture_create_gpu_data(RI_Assets *a, RI_Renderer *r);
void ri_assets_texture_send_to_gpu(RI_Assets *a, RI_Renderer *r);


#ifdef __cplusplus
}
#endif


#endif
