#include "ruin.h"
#include "cvec.h"
#include "ri_platform/ri_platform.h"
#include "ri_renderer/ri_renderer.h"
#include "ruin__internal.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_video.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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

    engine.platform.active_config  = c->window;
    engine.platform.pending_config = c->window;
    engine.renderer.active_config  = c->renderer;
    engine.renderer.pending_config = c->renderer;
    
    ri_platform_init(&engine.platform);
    ri_renderer_init(&engine.renderer, &engine.platform);

    engine.entities.next_entity_id = 1;
    engine.entities.dense_entities = (cvec){0};

    engine.components = (RI_Components){0};
/*
    engine.components.transforms.sparse_data      = malloc(RUIN_MAX_ENTITIES * sizeof(RTransform));
    engine->components.renderer_2ds.sparse_data    = malloc(RUIN_MAX_ENTITIES * sizeof(RSpriteRenderer));
    engine->components.renderer_3ds.sparse_data    = malloc(RUIN_MAX_ENTITIES * sizeof(RMeshRenderer));
    engine->components.sound_players.sparse_data   = malloc(RUIN_MAX_ENTITIES * sizeof(RSoundPlayer));
    engine->components.music_players.sparse_data   = malloc(RUIN_MAX_ENTITIES * sizeof(RMusicPlayer));
    engine->components.sound_listeners.sparse_data = malloc(RUIN_MAX_ENTITIES * sizeof(RSoundListener));
    engine->components.camera_2ds.sparse_data      = malloc(RUIN_MAX_ENTITIES * sizeof(RCamera2D));
    engine->components.camera_3ds.sparse_data      = malloc(RUIN_MAX_ENTITIES * sizeof(RCamera3D));
*/

    ri_comp_transform_init(&engine);

    return 1;
}

void rnSelfKill() {
    ri_renderer_kill(&engine.renderer);
    ri_platform_kill(&engine.platform);

    free(engine.components.transforms.sparse_data);
/*
    free(engine->components.renderer_2ds.sparse_data);
    free(engine->components.renderer_3ds.sparse_data);
    free(engine->components.sound_players.sparse_data);
    free(engine->components.music_players.sparse_data);
    free(engine->components.sound_listeners.sparse_data);
    free(engine->components.camera_2ds.sparse_data);
    free(engine->components.camera_3ds.sparse_data);
*/
    ri_comp_transform_kill(&engine);
}




RnBool rnSelfRunning() {
    return ri_platform_window_running(&engine.platform);
}



void rnFrameBegin() {
    ri_platform_time_update(&engine.platform);
}

void rnFrameEnd() {

    engine.platform.window.resized = 0;

    ri_comp_transform_update(&engine);
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

    draw_frame(&engine.renderer, &engine.platform);

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

float ruinMouseScroll() {
    return engine.platform.input.mouse_scroll;
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

    engine.platform.time.target_delta = 1.0 / (float)t;
}

RnEntity rnEntityCreate() {
    cvec_push(engine.entities.dense_entities, engine.entities.next_entity_id, RnEntity);
    engine.entities.next_entity_id += 1;

    return engine.entities.next_entity_id - 1;
}

void rnEntityKill(RnEntity e) {
    cvec_remove(engine.entities.dense_entities, e, RnEntity);
}

RnBool rnEntityValid(RnEntity e) {
    RnEntity id = RUIN_INVALID_ID;
    cvec_index(engine.entities.dense_entities, e, RnEntity, id);

    return id;
}


RnTransform ruinDefaultTransform() {
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
    cvec_remove_at(engine.components.transforms.dense_indices, e, RnEntity);
}
RnTransform *rnEntityTransformGet(RnEntity e) {
#ifdef RUIN_ENABLE_DEBUG
    RnEntity id = (RnEntity)-1;
    cvec_index(engine.entities.dense_entities, e, RnEntity, id);

    if (id == (RnEntity)-1) {
        RUIN_DEBUG("Failed to get RTansform of Entity #%d", e);
        return NULL;
    }
#endif
    
    return &(void_t(engine.components.transforms.sparse_data, RnTransform)[e]);
}
RnVec3 rnEntityTransformGetWorldPosition(RnEntity e) {
    vec3 *v = &(engine.component_internals.transform_internal.sparse_world_positions[e]);

    RUIN_DEBUG("addr: %p", v);

    return (RnVec3){
        .x = (*v)[0],
        .y = (*v)[1],
        .z = (*v)[2]
    };
}
RnVec3 rnEntityTransformGetWorldRotation(RnEntity e) {
    vec3 *v = &(engine.component_internals.transform_internal.sparse_world_rotations[e]);

    return (RnVec3){
        .x = (*v)[0],
        .y = (*v)[1],
        .z = (*v)[2]
    };
}
RnVec3 rnEntityTransformGetWorldScale(RnEntity e) {
    vec3 *v = &(engine.component_internals.transform_internal.sparse_world_scales[e]);

    return (RnVec3){
        .x = (*v)[0],
        .y = (*v)[1],
        .z = (*v)[2]
    };
}








