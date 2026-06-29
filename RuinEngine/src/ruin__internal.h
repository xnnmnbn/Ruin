#ifndef RUIN_INTERNAL_H
#define RUIN_INTERNAL_H

#include <SDL3/SDL_mouse.h>
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <stdint.h>

#include "ri_platform/ri_platform.h"
#include "ri_renderer/ri_renderer.h"
#include "ri_assets/ri_assets.h"
#include "ri_components/ri_components.h"



#ifdef __cplusplus
extern "C" {
#endif

#include "ruin.h"

#include <SDL3/SDL.h>

#define void_t(d, t) ((t*)(d))


typedef struct {
    RnConfig    active_config;
    RnConfig    pending_config;

    RI_Platform   platform;
    RI_Renderer   renderer;
    RI_Assets     assets;
    RI_Components components;
} RuinInternal;


void record_command(RI_Renderer *r, RI_Platform *p, VkCommandBuffer cmd, uint32_t img_idx);
void record_command_2d(RI_Renderer *r, RI_Platform *p, RI_Components *c, VkCommandBuffer cmd, uint32_t img_idx);

void draw_frame(RuinInternal *engine);


void ri_entities_init(RuinInternal *engine);
void ri_entities_kill(RuinInternal *engine);


#ifdef __cplusplus
}
#endif

#endif
