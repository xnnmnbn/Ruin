#include "ruin.h"
#include "cvec.h"
#include "ri_assets/ri_assets.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ri_components/ri_components.h"
#include "ri_platform/ri_platform.h"
#include "ri_renderer/ri_renderer.h"
#include "ruin__internal.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_video.h>
#include <cglm/cglm.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>

static RuinInternal engine = {0};


RnConfig *rnConfigGet() {
    return &engine.pending_config;
}

void rnConfigUpdatePlatform() {
    SDL_SetWindowTitle(engine.platform.window.window, engine.pending_config.window.title);
    SDL_SetWindowFullscreen(engine.platform.window.window, engine.pending_config.window.fullscreen);
    SDL_SetWindowBordered(engine.platform.window.window, !engine.pending_config.window.borderless);
    SDL_SetWindowResizable(engine.platform.window.window, engine.pending_config.window.resizable);
    engine.platform.active_config = engine.pending_config.window;
    engine.platform.window.resized = 1;
}


void rnConfigUpdateRenderer() {
    engine.renderer.sync.current_frame = 0;
    engine.renderer.sync.skip_wait     = 1;
    engine.renderer.active_config      = engine.pending_config.renderer;
}










RnBool rnSelfInit(RnConfig *c) {

    if (!c) {
        printf("No config provided.\n");
        return 0;
    }

    printf("Size of engine is %zu bytes.\n", sizeof(engine));

    engine.platform.active_config  = c->window;
    engine.platform.pending_config = c->window;
    engine.renderer.active_config  = c->renderer;
    engine.renderer.pending_config = c->renderer;
    
    ri_platform_init(&engine.platform);
    ri_renderer_init(&engine.renderer, &engine.platform);
    ri_assets_init(&engine.assets);
    ri_components_init(&engine.components);

    printf("%d.\n", engine.components.next_entity_id);

    // exit(0);

    return 1;
}

void rnSelfKill() {
    ri_assets_kill(&engine.assets, &engine.renderer);
    ri_components_kill(&engine.components);
    ri_renderer_kill(&engine.renderer);
    ri_platform_kill(&engine.platform);
}

RnBool rnSelfRunning() {
    return ri_platform_window_running(&engine.platform);
}










void rnFrameBegin() {
    ri_platform_time_update(&engine.platform);
/*
RnEntity e0 = from_void(engine.components.entities.data, RnEntity)[0];
RnEntity e1 = from_void(engine.components.entities.data, RnEntity)[1];
printf("entities[0]=%u entities[1]=%u\n", e0, e1);

RnMaterial2D mid = from_void(engine.renderer.gpu_buffers_2d.material_idx_ssbo.mapped, RnMaterial2D)[1];
printf("material_ids[1]=%u\n", mid);

RnMaterial2DInfo *mat = &(from_void(engine.renderer.gpu_buffers_2d.material_ssbo.mapped, RnMaterial2DInfo)[1]);
printf("tint=%.2f,%.2f,%.2f,%.2f tex_id=%u\n", mat->tint.r, mat->tint.g, mat->tint.b, mat->tint.a, mat->texture);

RnTexture tex_id = from_void(engine.renderer.gpu_buffers_2d.material_idx_ssbo.mapped, RnMaterial2D)[mid];
printf("tex_id from material=%u\n", mat->texture);
*/
}

void rnFrameEnd() {
/*
    double remaining = engine.platform.time.target_delta - engine.platform.time.fixed_delta;
    uint64_t del = (uint64_t)(remaining * 100000000);

    if (remaining > 0) {
        SDL_DelayPrecise(del);
    }
*/

    if (engine.platform.window.resized) {

        if (engine.platform.active_config.width == 0 || engine.platform.active_config.height == 0) {
            printf("%d, %d\n", engine.platform.active_config.width, engine.platform.active_config.height);
            return;
        }
        ri_renderer_recreate_swapchain(&engine.renderer, &engine.platform);
        engine.platform.window.resized = 0;
        return;
    }

    if (engine.platform.active_config.width == 0 || engine.platform.active_config.height == 0) {
        return;
    }

    ri_components_update(&engine.components);

    draw_frame(&engine);


    engine.platform.window.resized = 0;
}










RnBool rnKeyDown(RnKey k) {
    return ( engine.platform.input.keys_now[k]) &&
           (!engine.platform.input.keys_was[k]);
}
RnBool rnKeyHold(RnKey k) {
    return engine.platform.input.keys_now[k];
}
RnBool rnKeyUp(RnKey k) {
    return ( engine.platform.input.keys_was[k]) &&
           (!engine.platform.input.keys_now[k]);
}

RnBool rnMouseDown(RnMouseButton b) {
    return ( engine.platform.input.mouse_now[b - 1]) &&
           (!engine.platform.input.mouse_was[b - 1]);
}
RnBool rnMouseHold(RnMouseButton b) {
    return engine.platform.input.mouse_now[b - 1];
}
RnBool rnMouseUp(RnMouseButton b) {
    return ( engine.platform.input.mouse_was[b - 1]) &&
           (!engine.platform.input.mouse_now[b - 1]);
}

float rnMouseScroll() {
    return engine.platform.input.mouse_scroll;
}

RnVec2 rnMouseMove() {
    return (RnVec2){
        .x = engine.platform.input.mouse_dx,
        .y = engine.platform.input.mouse_dy
    };
}










uint8_t rnTimeFPS() {
    return engine.platform.time.fps;
}
float rnTimeDelta() {
    return engine.platform.time.delta;
}
float rnTimeDeltaFixed() {
    return engine.platform.time.fixed_delta;
}
float rnTimeElapsed() {
    return engine.platform.time.elapsed;
}
float rnTimeElapsedFixed() {
    return engine.platform.time.fixed_elapsed;
}
void rnTimeSetSpeed(float s) {
    engine.platform.time.speed = s;
}
void rnTimeSetTargetFPS(uint8_t t) {
    if (t == 0) {
        engine.platform.time.target_delta = 0;
        return;
    }

    engine.platform.time.target_delta = 1.0 / (double)t;
}










RnEntity rnEntityCreate() {
    RnEntity next_id = engine.components.next_entity_id;
    
    cvec_push(engine.components.entities, next_id, RnEntity);
    engine.components.next_entity_id += 1;

    return next_id;
}

void rnEntityKill(RnEntity e) {
    cvec_remove(engine.components.entities, e, RnEntity);
}

RnBool rnEntityValid(RnEntity e) {
    RnEntity id = RUIN_INVALID_ID;
    cvec_index(engine.components.entities, e, RnEntity, id);

    if (id != RUIN_INVALID_ID) {
        return 1;
    }

    return RUIN_INVALID_ID;
}










RnMaterial2DInfo rnDefaultMaterial2DInfo() {
    return (RnMaterial2DInfo) {
        ._pad = {0},
        .texture = 0,
        .tint = (RnColor) {
            .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f
        }
    };
}

RnMaterial2D rnMaterial2DCreate(RnMaterial2DInfo i) {
    from_void(engine.assets.material2Ds.data, RnMaterial2DInfo)[
        engine.assets.material2Ds.next_idx
    ] = i;

    cvec_push(engine.assets.material2Ds.valid_indices, engine.assets.material2Ds.next_idx, RnMaterial2D);

    engine.assets.material2Ds.next_idx += 1;

    return engine.assets.material2Ds.next_idx - 1;
}

RnMaterial2DInfo *rnMaterial2DGet(RnMaterial2D m) {
    return &(from_void(engine.assets.material2Ds.data, RnMaterial2DInfo)[m]);
}

void rnMaterial2DKill(RnMaterial2D m) {
    cvec_remove(engine.assets.material2Ds.valid_indices, m, RnMaterial2D);
}



RnMaterial3DInfo rnDefaultMaterial3DInfo() {
    return (RnMaterial3DInfo) {0};
}

RnMaterial3D rnMaterial3DCreate(RnMaterial3DInfo i) {
    from_void(engine.assets.material3Ds.data, RnMaterial3DInfo)[
        engine.assets.material3Ds.next_idx
    ] = i;

    cvec_push(engine.assets.material3Ds.valid_indices, engine.assets.material3Ds.next_idx, RnMaterial3D);

    engine.assets.material3Ds.next_idx += 1;

    return engine.assets.material3Ds.next_idx - 1;;
}

RnMaterial3DInfo *rnMaterial3DGet(RnMaterial3D m) {
    return &(from_void(engine.assets.material3Ds.data, RnMaterial3DInfo)[m]);
}

void rnMaterial3DKill(RnMaterial3D m) {
    cvec_remove(engine.assets.material3Ds.valid_indices, m, RnMaterial3D);
}










RnTexture rnTextureCreate(const char *path) {
    RnTexture next_idx = engine.assets.cpu_textures.next_idx;

    int w, h, ch;

    stbi_uc *pixels = stbi_load(path, &w, &h, &ch, STBI_rgb_alpha);

    if (!pixels) {
        printf("Failed to load Texture to CPU: %s.\n", path);
        return  0;
    }
    
    from_void(engine.assets.cpu_textures.data, RI_Asset_Texture_CPU)[next_idx] = (RI_Asset_Texture_CPU){
        .width = w,
        .height = h,
        .channels = 4,
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .pixels = pixels
    };

    cvec_push(engine.assets.cpu_textures.valid_indices, next_idx, RnTexture);

    engine.assets.cpu_textures.next_idx += 1;

    printf("Texture loaded to CPU: %s\n", path);

    return  next_idx;
}

void rnTextureCreateGPUResources() {
    ri_assets_texture_create_gpu_data(&engine.assets, &engine.renderer);
}

void rnTextureLoadAllToGPU() {
    uint32_t t_count = engine.assets.gpu_textures.valid_indices.len;

    VkWriteDescriptorSet writes[t_count];
    VkDescriptorImageInfo img_infos[t_count];
    
    for (uint32_t i = 0; i < t_count; i++) {

        RnTexture t = from_void(engine.assets.gpu_textures.valid_indices.data, RnTexture)[i];

        printf("Tex: %d\n", t);
        
        img_infos[i].sampler = from_void(engine.assets.gpu_textures.data, RI_Asset_Texture_GPU)[t].sampler;
        img_infos[i].imageView = from_void(engine.assets.gpu_textures.data, RI_Asset_Texture_GPU)[t].view;
        img_infos[i].imageLayout = from_void(engine.assets.gpu_textures.data, RI_Asset_Texture_GPU)[t].layout;
        
        writes[i] = (VkWriteDescriptorSet){0};
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].dstSet = engine.renderer.pipelines.bindless_pipeline_2d.d_sets[1];
        writes[i].dstBinding = 0;
        writes[i].dstArrayElement = t;
        writes[i].descriptorCount = 1;
        writes[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[i].pImageInfo = &img_infos[i];

printf("updating %u texture descriptors, t=%u, view=%p, sampler=%p\n",
    t_count, t,
    img_infos[0].imageView,
    img_infos[0].sampler);
    }

    vkUpdateDescriptorSets(engine.renderer.core.device, t_count, writes, 0, NULL);
}











RnTransform rnDefaultTransform() {
    return (RnTransform){
        .position = {0},
        .rotation = {0},
        .scale    = { 1.0f, 1.0f, 1.0f },
        .parent   = 0,
        .dirty    = 0
    };
}

void rnEntityTransformAdd(RnEntity e, RnTransform t) {
   ((RnTransform*)engine.components.transforms.sparse_data)[e] = t;
   cvec_push(engine.components.transforms.dense_indices, e, RnEntity);
}
void rnEntityTransformKill(RnEntity e) {
    cvec_remove(engine.components.transforms.dense_indices, e, RnEntity);
}
RnTransform *rnEntityTransformGet(RnEntity e) {
#ifdef RUIN_ENABLE_DEBUG
    RnEntity id = (RnEntity)-1;
    cvec_index(engine.components.entities, e, RnEntity, id);

    if (id == (RnEntity)-1) {
        RUIN_DEBUG("Failed to get RTansform of Entity #%d", e);
        return NULL;
    }
#endif
    
    return &(void_t(engine.components.transforms.sparse_data, RnTransform)[e]);
}
RnVec3 rnEntityTransformGetWorldPosition(RnEntity e) {
    mat4 *m = &(engine.components.i_transforms.world_matrices[e]);
    vec3 v;

    float *mm = (float*)*m;

    glm_vec3_copy((*m)[3], v);

    return (RnVec3){
        v[0], v[1], v[2]
    };
}

RnVec3 rnEntityTransformGetWorldRotation(RnEntity e) {
    mat4 *m = &(engine.components.i_transforms.world_matrices[e]);
    vec3 v;

    glm_euler_angles(*m, v);

    return (RnVec3){
        .x = glm_rad((v)[0]),
        .y = glm_rad((v)[1]),
        .z = glm_rad((v)[2])
    };
}

RnVec3 rnEntityTransformGetWorldScale(RnEntity e) {
    vec3 *v = &(engine.components.i_transforms.world_scales[e]);

    return (RnVec3){
        .x = (*v)[0],
        .y = (*v)[1],
        .z = (*v)[2]
    };
}










void rnEntityCamera2DAdd(RnEntity e, RnCamera2D c) {
    from_void(engine.components.camera2ds.sparse_data, RnCamera2D)[e] = c;
    cvec_push(engine.components.camera2ds.dense_indices, e, RnEntity);
}

void rnEntityCamera2DKill(RnEntity e) {
    cvec_remove(engine.components.camera2ds.dense_indices, e, RnEntity);
}

void rnEntityCamera2DUse(RnEntity e) {
    engine.components.camera_in_use = e;
    engine.components.camera_mode = 2;
}

RnCamera2D *rnEntityCamera2DGet(RnEntity e) {
    return &(from_void(engine.components.camera2ds.sparse_data, RnCamera2D)[e]);
}

void rnEntityCamera3DAdd(RnEntity e, RnCamera3D c) {
    from_void(engine.components.camera3ds.sparse_data, RnCamera3D)[e] = c;
    cvec_push(engine.components.camera3ds.dense_indices, e, RnEntity);
}

void rnEntityCamera3DKill(RnEntity e) {
    cvec_remove(engine.components.camera3ds.dense_indices, e, RnEntity);
}

void rnEntityCamera3DUse(RnEntity e) {
    engine.components.camera_in_use = e;
    engine.components.camera_mode = 3;
}

RnCamera3D *rnEntityCamera3DGet(RnEntity e) {
    return &(from_void(engine.components.camera3ds.sparse_data, RnCamera3D)[e]);
}










RnSpriteRenderer *rnEntitySpriteRendererGet(RnEntity e) {
    return &(from_void(engine.components.sprite_renderers.sparse_data, RnSpriteRenderer)[e]);
}

void rnEntitySpriteRendererAdd(RnEntity e, RnSpriteRenderer r) {
    from_void(engine.components.sprite_renderers.sparse_data, RnSpriteRenderer)[e] = r;
    cvec_push(engine.components.sprite_renderers.dense_indices, e, RnEntity);
}

void rnEntitySpriteRendererKill(RnEntity e) {
    cvec_remove(engine.components.sprite_renderers.dense_indices, e, RnEntity);
}








