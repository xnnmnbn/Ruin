#ifndef RUIN_ASSETS_H
#define RUIN_ASSETS_H


#include "cvec.h"
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#include "rn_renderer/rn_renderer.h"

#include "vk_mem_alloc.h"
#include "miniaudio.h"

#define MAX_SOUND_REF_COUNT 16


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void    *data;
    cvec     valid_indices;
    cvec     free_indices;
} RN_AssetStorage;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    VkFormat format;
    uint8_t *pixels;
    uint32_t gpu_idx;
} RN_Asset_Texture_CPU;

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
} RN_Asset_Texture_GPU;


typedef struct {
    ma_sound sounds[MAX_SOUND_REF_COUNT];
    uint32_t current_ref;
} RN_Asset_Sound;



typedef struct {
    RN_AssetStorage sounds;
    RN_AssetStorage musics;
    
    RN_AssetStorage material2Ds;

    RN_AssetStorage cpu_textures;
    RN_AssetStorage gpu_textures;

    // RN_AssetStorage gui_rects;
} RN_Assets;



uint32_t rn_assetstorage_idx(RN_AssetStorage *s);
void     rn_assetstorage_update(RN_AssetStorage *s);
void     rn_assetstorage_kill(RN_AssetStorage *s, uint32_t idx);




void rn_assets_init(RN_Assets *a);
void rn_assets_kill(RN_Assets *a, RN_Renderer *r);

void rn_assets_audio_init(RN_Assets *a);
void rn_assets_audio_kill(RN_Assets *a);
/*
void rn_assets_material_init(RN_Assets *a);
void rn_assets_material_kill(RN_Assets *a);
*/
void rn_assets_texture_init(RN_Assets *a);
void rn_assets_texture_kill(RN_Assets *a, RN_Renderer *r);

void rn_assets_rendertarget_create(RN_Assets *a, RN_Renderer *r, uint32_t w, uint32_t h);
void rn_assets_rendertarget_kill(RN_Assets *a, RN_Renderer *r);

void rn_assets_texture_create_gpu_data(RN_Assets *a, RN_Renderer *r);
void rn_assets_texture_send_to_gpu(RN_Assets *a, RN_Renderer *r);

/*
void rn_assets_gui_rect_init(RN_Assets *a);
void rn_assets_gui_rect_kill(RN_Assets *a);
*/

#ifdef __cplusplus
}
#endif


#endif
