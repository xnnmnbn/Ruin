#ifndef RUIN_INTERNAL_H
#define RUIN_INTERNAL_H

#include <SDL3/SDL_mouse.h>
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <stdint.h>

#include "ri_platform/ri_platform.h"
#include "ri_renderer/ri_renderer.h"



#ifdef __cplusplus
extern "C" {
#endif

#include "ruin.h"
#include "cvec.h"

#include <SDL3/SDL.h>
#include <cglm/cglm.h>

#define void_t(d, t) ((t*)(d))


// sparse data never dies unless game ends
// data of entity e means sparse_data[e] if e exists in dense data
// zero is always invalid
typedef struct {
    cvec  dense_indices;
    void *sparse_data;
} RI_Storage;


typedef struct {
    vec3 *sparse_world_positions;
    vec3 *sparse_world_rotations;
    vec3 *sparse_world_scales;
    
    mat4 *sparse_local_matrices;
    mat4 *sparse_world_matrices;
    mat4 *dense_matrices_to_gpu;

    cvec  dense_indices_for_cpu;
    cvec  dense_indices_for_gpu;
    
} RI_TransformInternal;


typedef struct {
    RI_Storage transforms;
    RI_Storage sound_listeners;
    RI_Storage sound_players;
    RI_Storage music_players;
    RI_Storage camera_2ds;
    RI_Storage camera_3ds;
    RI_Storage renderer_2ds;
    RI_Storage renderer_3ds;
} RI_Components;

typedef struct {
    RI_TransformInternal transform_internal;
} RI_ComponentInternals;

typedef struct {
    RI_Storage textures;
    RI_Storage sounds;
    RI_Storage musics;
    RI_Storage meshes;
    RI_Storage render_targets;
    RI_Storage material_2ds;
    RI_Storage material_3ds;
} RI_Assets;

typedef struct {
    cvec      dense_entities;  // iterator
    RnEntity  next_entity_id;  // counter never decreases
} RI_Entities;





typedef struct RuinInternal {
    RnConfig    active_config;
    RnConfig    pending_config;

    RI_Platform platform;
    RI_Renderer renderer;

    RI_Entities   entities;
    RI_Components components;
    RI_ComponentInternals component_internals;
    RI_Assets     assets;

} RuinInternal;


void ri_entities_init(RuinInternal *engine);
void ri_entities_kill(RuinInternal *engine);



void ri_components_init(RuinInternal *engine);
void ri_components_kill(RuinInternal *engine);
void ri_assets_init(RuinInternal *engine);
void ri_assets_kill(RuinInternal *engine);


void ri_comp_transform_init(RuinInternal *engine);
void ri_comp_transform_update(RuinInternal *engine);
void ri_comp_transform_kill(RuinInternal *engine);



#ifdef __cplusplus
}
#endif

#endif
