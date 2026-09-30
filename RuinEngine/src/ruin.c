#include "ruin.h"
#include "cvec.h"
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <assert.h>
#include <stdlib.h>
#include "rn_assets/rn_assets.h"
#include "rn_components/rn_components.h"
#include "rn_engine.h"
#include "rn_renderer/rn_renderer.h"
#include "stb_image.h"
#include "stb_image_resize2.h"
#include "rn_engine.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_video.h>
#include <cglm/cglm.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <vulkan/vulkan_core.h>

static RuinEngine engine = {0};


RnConfig rnDefaultConfig() {
    return (RnConfig){
        .window = (RnConfigWindow){
            .borderless = 0,
            .fullscreen = 0,
            .resizable = 0,
            .width = 800,
            .height = 450,
            .title = "Ruin Test",
        },
        .renderer = (RnConfigRenderer){
            .max_anisotropy = 1.0f,
            .max_frames_in_flight = 2,
            .multisampling = 1,
            .resolution_x = 1280,
            .resolution_y = 720,
            .vsync = 0,
        },
        .audio = (RnConfigAudio){
            .music_volume = 1.0,
            .sound_volume = 1.0,
        },
    };
}


RnConfig *rnConfigGet() {
    return &engine.pending_config;
}

void rnConfigUpdatePlatform() {
    SDL_SetWindowTitle(engine.platform.window.window, engine.pending_config.window.title);
    SDL_SetWindowFullscreen(engine.platform.window.window, engine.pending_config.window.fullscreen);
    SDL_SetWindowBordered(engine.platform.window.window, !engine.pending_config.window.borderless);
    SDL_SetWindowResizable(engine.platform.window.window, engine.pending_config.window.resizable);
    engine.platform.active_config = engine.pending_config.window;
    engine.active_config.window = engine.pending_config.window;
    engine.platform.window.resized = 1;
}


void rnConfigUpdateRenderer() {
    engine.renderer.sync.current_frame = 0;
    engine.renderer.active_config      = engine.pending_config.renderer;

    rn_renderer_recreate_swapchain(&engine.renderer, &engine.platform);
}


void rnConfigUpdateAll() {
    rnConfigUpdatePlatform();
    rnConfigUpdateRenderer();
}










RnBool rnSelfInit(RnConfig *c) {

    if (!c) {
        printf("No config provided.\n");
        return 0;
    }

    RUIN_DEBUG("Size of engine is %zu bytes.\n", sizeof(engine));

    static_assert((sizeof(RnRenderer2D) == 64), "fuck");

    srand(time(nullptr));

    engine.pending_config = *c;
    engine.active_config = *c;

    engine.platform.active_config  = c->window;
    engine.platform.pending_config = c->window;
    engine.renderer.active_config  = c->renderer;
    engine.renderer.pending_config = c->renderer;
    
    rn_platform_init(&engine.platform);
    rn_audio_init(&engine.audio);
    rn_renderer_init(&engine.renderer, &engine.platform);
    rn_assets_init(&engine.assets);
    rn_components_init(&engine.components);

    // exit(0);

    return 1;
}

void rnSelfKill() {
    rn_components_kill(&engine.components);
    rn_assets_kill(&engine.assets, &engine.renderer);
    rn_renderer_kill(&engine.renderer);
    rn_audio_kill(&engine.audio, &engine.assets);
    rn_platform_kill(&engine.platform);
}

RnBool rnSelfRunning() {
    return rn_platform_window_running(&engine.platform);
}










RnSound rnSoundLOad(const char *path) {
    RnSound idx = rn_assetstorage_idx(&engine.assets.sounds);

    for (uint8_t i = 0; i < MAX_SOUND_REF_COUNT; i++) {
        if (ma_sound_init_from_file(
            &engine.audio.engine,
            path,
            MA_SOUND_FLAG_DECODE,
            NULL, NULL,
            &((from_void(engine.assets.sounds.data, RN_Asset_Sound)[idx].sounds[i])
        )) != MA_SUCCESS) {
            return 0;
        }
    }

    rn_assetstorage_update(&engine.assets.sounds);

    return idx;
}


RnMusic rnMusicLoad(const char *path) {
    RnMusic idx = rn_assetstorage_idx(&engine.assets.musics);

    if (ma_sound_init_from_file(
        &engine.audio.engine,
        path,
        MA_SOUND_FLAG_STREAM,
        NULL, 0,
        &(from_void(engine.assets.musics.data, ma_sound)[idx]
    )) != MA_SUCCESS) {
        return 0;
    }

    rn_assetstorage_update(&engine.assets.musics);

    return idx;
}


void rnSoundPlay(RnSound s) {
    RN_Asset_Sound *sound = &(from_void(engine.assets.sounds.data, RN_Asset_Sound)[s]);
    ma_sound_start(&(sound->sounds[sound->current_ref]));
    sound->current_ref = (sound->current_ref + 1) % MAX_SOUND_REF_COUNT;
}

void rnMusicPlay(RnMusic m) {
    ma_sound_start(&(from_void(engine.assets.musics.data, ma_sound)[m]));
}





RnPostProcess rnDefaultPostProcess() {
    return (RnPostProcess) {
        .brightness = 1.0f,
        .saturation = 1.0f,
        .contrast   = 1.0f,
        .invert     = 0,
        .tint       = (RnColor) {
            1.0, 1.0, 1.0, 1.0
        }
    };
}

RnPostProcess *rnPostProcessGet() {
    return &engine.renderer.post_process;
}










void rnFrameBegin() {
    rn_platform_time_update(&engine.platform);
}

void rnFrameEnd() {
    double remaining = engine.platform.time.target_delta - engine.platform.time.fixed_delta;
    uint64_t del = (uint64_t)(remaining * 2000000000);

    if (remaining > 0) {
        if (engine.platform.time.target_delta != 0) SDL_DelayPrecise(del);
    }


    if (engine.platform.window.resized) {

        if (engine.platform.active_config.width == 0 || engine.platform.active_config.height == 0) {
            RUIN_DEBUG("%d, %d\n", engine.platform.active_config.width, engine.platform.active_config.height);
            return;
        }
        rn_renderer_recreate_swapchain(&engine.renderer, &engine.platform);
        engine.platform.window.resized = 0;
        return;
    }

    if (engine.platform.active_config.width == 0 || engine.platform.active_config.height == 0) {
        return;
    }

    
    rn_components_update(&engine.components);

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
        .y = -engine.platform.input.mouse_dy
    };
}

void rnMouseHideCursor(RnBool b) {
    SDL_SetWindowRelativeMouseMode(engine.platform.window.window, b);

    if (b) {
        SDL_HideCursor();
    } else {
        SDL_ShowCursor();
    }
}










uint32_t rnTimeFPS() {
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









/*
RnMaterial2DInfo rnDefaultMaterial2DInfo() {
    return (RnMaterial2DInfo) {
        .texture = 0,
        .brightness = 0.0,
        .saturation = 1.0,
        .contrast   = 1.0,
        .inverse    = 0.0,
        .bloom      = 1.0,
        .glitch     = 0.0,
        .chromatic_aberration = 0.0,
        .tint = (RnColor) {
            .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f
        }
    };
}

RnMaterial2D rnMaterial2DCreate(RnMaterial2DInfo i) {
    uint32_t idx = rn_assetstorage_idx(&engine.assets.material2Ds);
    from_void(engine.assets.material2Ds.data, RnMaterial2DInfo)[idx] = i;
    rn_assetstorage_update(&engine.assets.material2Ds);

    return idx;
}

RnMaterial2DInfo *rnMaterial2DGet(RnMaterial2D m) {
    return &(from_void(engine.assets.material2Ds.data, RnMaterial2DInfo)[m]);
}

void rnMaterial2DKill(RnMaterial2D m) {
    rn_assetstorage_kill(&engine.assets.material2Ds, m);
}
*/













RnTexture rnDefaultTexture() {
    return 0;
}

RnTexture rnTextureCreate(const char *path, uint32_t w, uint32_t h) {
    RnTexture cpu_next_idx = rn_assetstorage_idx(&engine.assets.cpu_textures);
    RnTexture gpu_next_idx = rn_assetstorage_idx(&engine.assets.gpu_textures);

    int _w, _h, _ch;

    stbi_uc *pixels = stbi_load(path, &_w, &_h, &_ch, STBI_rgb_alpha);
    if (!pixels) {
        RUIN_DEBUG("Failed to load Texture to CPU: %s.\n", path);
        return  0;
    }

    
    if (w == 0 && h == 0) {
        from_void(engine.assets.cpu_textures.data, RN_Asset_Texture_CPU)[cpu_next_idx] = (RN_Asset_Texture_CPU){
            .width = _w,
            .height = _h,
            .channels = 4,
            .format = VK_FORMAT_R8G8B8A8_SRGB,
            .pixels = pixels,
            .gpu_idx = gpu_next_idx
        };
    } else {
        stbi_uc *res_pixels = malloc(
            ((w == 0) ? _w : w) *
            ((h == 0) ? _h : h) *
            4
        );

        uint8_t *res = stbir_resize_uint8_linear(
            pixels,
            _w, _h, 0, res_pixels,
            (w == 0 ? _w : w),
            (h == 0 ? _h : h),
            0, 4
        );

        if (!res) {
            printf("Failed to resize image: %s\n", path);
            return 0;
        }
        from_void(engine.assets.cpu_textures.data, RN_Asset_Texture_CPU)[cpu_next_idx] = (RN_Asset_Texture_CPU){
            .width = (w == 0 ? _w : w),
            .height = (h == 0 ? _h : h),
            .channels = 4,
            .format = VK_FORMAT_R8G8B8A8_SRGB,
            .pixels = res_pixels,
            .gpu_idx = gpu_next_idx
        };
    }




    rn_assetstorage_update(&engine.assets.cpu_textures);
    rn_assetstorage_update(&engine.assets.gpu_textures);

    RUIN_DEBUG("Texture loaded to CPU: %s\n", path);
    RUIN_DEBUG("CPU idx: %d\n", cpu_next_idx);

    return gpu_next_idx;
}

void rnTextureCreateGpuResources() {
    rn_assets_texture_create_gpu_data(&engine.assets, &engine.renderer);
}

void rnTextureLoadToGpuOffscreen(RnTexture *textures, uint32_t count) {

    uint32_t t_count = 0;
    RnTexture *tex_ids = NULL;

    if (textures == NULL || count == 0) {
        t_count = engine.assets.gpu_textures.valid_indices.len;
        tex_ids = engine.assets.gpu_textures.valid_indices.data;
    } else {
        t_count = count;
        tex_ids = textures;
    }

    VkWriteDescriptorSet writes[t_count];
    VkDescriptorImageInfo img_infos[t_count];


    
    for (uint32_t i = 0; i < t_count; i++) {

        RnTexture t = from_void(tex_ids, RnTexture)[i];

        RUIN_DEBUG("Tex: %d\n", t);
        
        img_infos[i].sampler     = from_void(engine.assets.gpu_textures.data, RN_Asset_Texture_GPU)[t].sampler;
        img_infos[i].imageView   = from_void(engine.assets.gpu_textures.data, RN_Asset_Texture_GPU)[t].view;
        img_infos[i].imageLayout = from_void(engine.assets.gpu_textures.data, RN_Asset_Texture_GPU)[t].layout;
        
        writes[i] = (VkWriteDescriptorSet){0};
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].dstSet = engine.renderer.pipelines.bindless_offscreen_2d.d_sets[1];
        writes[i].dstBinding = 0;
        writes[i].dstArrayElement = t;
        writes[i].descriptorCount = 1;
        writes[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[i].pImageInfo = &img_infos[i];

        RUIN_DEBUG("updating %u texture descriptors, t=%u, view=%p, sampler=%p\n",
            t_count, t,
            img_infos[i].imageView,
            img_infos[i].sampler);
    }

    vkUpdateDescriptorSets(engine.renderer.core.device, t_count, writes, 0, NULL);

    VkWriteDescriptorSet write = {0};
    VkDescriptorImageInfo img_info = {0};

    img_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    img_info.imageView = engine.renderer.framebuffers.offscreen.color_view;
    img_info.sampler = engine.renderer.framebuffers.offscreen.color_sampler;

    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.dstSet = engine.renderer.pipelines.post_process_pipeline.d_sets[0];
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.pImageInfo = &img_info;

    vkUpdateDescriptorSets(engine.renderer.core.device, 1, &write, 0, NULL);


/*
    img_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    img_info.imageView = engine.renderer.framebuffers.post_process.color_view;
    img_info.sampler = engine.renderer.framebuffers.post_process.color_sampler;

    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.dstSet = engine.renderer.pipelines.gui_rect_pipeline.d_sets[1];
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.pImageInfo = &img_info;

    vkUpdateDescriptorSets(engine.renderer.core.device, 1, &write, 0, NULL);
*/
    RUIN_DEBUG("Descriptor Sets updated.\n");
}



/*
void rnTextureLoadToGpuGui(RnTexture *textures, uint32_t count) {

    uint32_t t_count = 0;
    RnTexture *tex_ids = NULL;

    if (textures == NULL || count == 0) {
        t_count = engine.assets.gpu_textures.valid_indices.len;
        tex_ids = engine.assets.gpu_textures.valid_indices.data;
    } else {
        t_count = count;
        tex_ids = textures;
    }

    VkWriteDescriptorSet writes[t_count];
    VkDescriptorImageInfo img_infos[t_count];


    
    for (uint32_t i = 0; i < t_count; i++) {

        RnTexture t = from_void(tex_ids, RnTexture)[i];

        RUIN_DEBUG("Tex: %d\n", t);
        
        img_infos[i].sampler     = from_void(engine.assets.gpu_textures.data, RN_Asset_Texture_GPU)[t].sampler;
        img_infos[i].imageView   = from_void(engine.assets.gpu_textures.data, RN_Asset_Texture_GPU)[t].view;
        img_infos[i].imageLayout = from_void(engine.assets.gpu_textures.data, RN_Asset_Texture_GPU)[t].layout;
        
        writes[i] = (VkWriteDescriptorSet){0};
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].dstSet = engine.renderer.pipelines.gui_rect_pipeline.d_sets[1];
        writes[i].dstBinding = 0;
        writes[i].dstArrayElement = t;
        writes[i].descriptorCount = 1;
        writes[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[i].pImageInfo = &img_infos[i];

        RUIN_DEBUG("Updating %u Texture Descriptors, t=%u, view=%p, sampler=%p\n",
            t_count, t,
            img_infos[i].imageView,
            img_infos[i].sampler);
    }

    vkUpdateDescriptorSets(engine.renderer.core.device, t_count, writes, 0, NULL);

    RUIN_DEBUG("Descriptor Sets updated.\n");
}
*/







RnTransform rnDefaultTransform() {
    return (RnTransform){
        .position = {0},
        .rotation = {0},
        .scale    = { 1.0f, 1.0f, 1.0f },
        .parent   = 0,
        .dead     = 0
    };
}

void rnTransformAdd(RnEntity e, RnTransform t) {
   from_void(engine.components.transforms.begin_points[0], RnTransform)[e] = t;
   RnEntity *entities  = engine.components.transforms.begin_points[2];
   uint32_t elem_count = engine.components.transforms.elem_counts;

   for (uint32_t i = 0; i < elem_count; i++) {
       if (entities[i] == e) return;
   }

   entities[elem_count] = e;
   engine.components.transforms.elem_counts += 1;
}
void rnTransformKill(RnEntity e) {
    RnEntity *entities  = engine.components.transforms.begin_points[2];
    uint32_t elem_count = engine.components.transforms.elem_counts;

    for (uint32_t i = 0; i < elem_count; i++) {
        if (entities[i] == e) {
            entities[i] = entities[elem_count - 1];
            engine.components.transforms.elem_counts -= 1;
            break;
        }
    }
}

RnTransform *rnTransformGet(RnEntity e) {
    return &(from_void(engine.components.transforms.begin_points[0], RnTransform)[e]);
}

RnVec3 rnTransformGetWorldPosition(RnEntity e) {
    mat4 *m = &(from_void(engine.components.transforms.begin_points[1], RN_Component_Transform)[e].world_matrix);
    vec3 v;

    float *mm = (float*)*m;

    glm_vec3_copy((*m)[3], v);

    return (RnVec3){
        v[0], v[1], v[2]
    };
}

RnVec3 rnTransformGetWorldRotation(RnEntity e) {
    mat4 *m = &(from_void(engine.components.transforms.begin_points[1], RN_Component_Transform)[e].world_matrix);
    vec3 v;

    glm_euler_angles(*m, v);

    return (RnVec3){
        .x = glm_rad((v)[0]),
        .y = glm_rad((v)[1]),
        .z = glm_rad((v)[2])
    };
}

RnVec3 rnTransformGetWorldScale(RnEntity e) {
    vec3 *v = &(from_void(engine.components.transforms.begin_points[1], RN_Component_Transform)[e].world_scale);

    return (RnVec3){
        .x = (*v)[0],
        .y = (*v)[1],
        .z = (*v)[2]
    };
}










void rnCamera2DAdd(RnEntity e, RnCamera2D c) {
    from_void(engine.components.camera2ds.begin_points[0], RnCamera2D)[e] = c;
    RnEntity *entities  = engine.components.camera2ds.begin_points[2];
    uint32_t elem_count = engine.components.camera2ds.elem_counts;

    for (uint32_t i = 0; i < elem_count; i++) {
        if (entities[i] == e) return;
    }

    entities[elem_count] = e;
    engine.components.camera2ds.elem_counts += 1;
}

void rnCamera2DKill(RnEntity e) {
    RnEntity *entities  = engine.components.camera2ds.begin_points[2];
    uint32_t elem_count = engine.components.camera2ds.elem_counts;

    for (uint32_t i = 0; i < elem_count; i++) {
        if (entities[i] == e) {
            entities[i] = entities[elem_count - 1];
            engine.components.camera2ds.elem_counts -= 1;
            break;
        }
    }
}

void rnCamera2DUse(RnEntity e) {
    engine.components.camera_in_use = e;
    engine.components.camera_mode = 2;
}

RnCamera2D *rnCamera2DGet(RnEntity e) {
    return &(from_void(engine.components.camera2ds.begin_points[0], RnCamera2D)[e]);
}










RnRenderer2D rnDefaultRenderer2D() {
    return (RnRenderer2D) {
        .aberration = {.x = 0.005F, .y = 0.005F},
        .aberration_strength = 0,
        .brightness = 1,
        .layer = 0,
        .texture = 0,
        .saturation = 1,
        .contrast = 1,
        .tint = { .r = 1.0F, .g = 1.0F, .b = 1.F, .a = 1.0F },
        .uv = { .x = 0.0F, .y = 0.0F, .w = 1.0F, .h = 1.0F },
    };
}

RnRenderer2D *rnRenderer2DGet(RnEntity e) {
    return &(from_void(engine.components.renderer2ds.begin_points[0], RnRenderer2D)[e]);
}

void rnRenderer2DAdd(RnEntity e, RnRenderer2D r) {
    from_void(engine.components.renderer2ds.begin_points[0], RnRenderer2D)[e] = r;
    RnEntity *entities  = engine.components.renderer2ds.begin_points[1];
    uint32_t elem_count = engine.components.renderer2ds.elem_counts;

    for (uint32_t i = 0; i < elem_count; i++) {
        if (entities[i] == e) return;
    }

    entities[elem_count] = e;
    engine.components.renderer2ds.elem_counts += 1;
}

void rnRenderer2DKill(RnEntity e) {
    RnEntity *entities  = engine.components.renderer2ds.begin_points[1];
    uint32_t elem_count = engine.components.renderer2ds.elem_counts;

    for (uint32_t i = 0; i < elem_count; i++) {
        if (entities[i] == e) {
            engine.components.renderer2ds.elem_counts -= 1;
            entities[i] = entities[elem_count - 1];
            break;
        }
    }
}

int sort_by_layer(const void *a, const void *b) {
    RnEntity e1 = *(RnEntity*)a;
    RnEntity e2 = *(RnEntity*)b;

    RnRenderer2D *r1 = &(from_void(engine.components.renderer2ds.begin_points[0], RnRenderer2D)[e1]);
    RnRenderer2D *r2 = &(from_void(engine.components.renderer2ds.begin_points[0], RnRenderer2D)[e2]);

    return (r2->layer < r1->layer) - (r1->layer < r2->layer);
}

void rnRenderer2DSortByLayer() {
    qsort(engine.components.renderer2ds.begin_points[1], engine.components.renderer2ds.elem_counts, sizeof(RnEntity), sort_by_layer);
}









/*
RnGuiRectInfo rnDefaultGuiRectInfo(void) {
    return (RnGuiRectInfo) {
        ._padding = 0,
        .layer = 0,
        .material = 0,
        .pivot = (RnVec2) {0},
        .position = (RnVec2) {0},
        .rotation = 0,
        .scale = (RnVec2) {
            .x = 160.0f,
            .y = 90.0f
        }
    };
}

RnGuiRect rnGuiRectCreate(RnGuiRectInfo r) {
    uint32_t idx = rn_assetstorage_idx(&engine.assets.gui_rects);
    from_void(engine.assets.gui_rects.data, RnGuiRectInfo)[idx] = r;
    rn_assetstorage_update(&engine.assets.gui_rects);

    return idx;
}

void rnGuiRectKill(RnGuiRect r) {
    rn_assetstorage_kill(&engine.assets.gui_rects, r);
}
*/





