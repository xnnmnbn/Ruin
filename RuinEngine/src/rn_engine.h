#ifndef RUIN_INTERNAL_H
#define RUIN_INTERNAL_H

#include <SDL3/SDL_mouse.h>
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <stdint.h>

#include "rn_platform/rn_platform.h"
#include "rn_renderer/rn_renderer.h"
#include "rn_audio/rn_audio.h"
#include "rn_assets/rn_assets.h"
#include "rn_components/rn_components.h"



#ifdef __cplusplus
extern "C" {
#endif

#include "ruin.h"

#include <SDL3/SDL.h>

#define void_t(d, t) ((t*)(d))


typedef struct {
    RnConfig    active_config;
    RnConfig    pending_config;

    RN_Platform   platform;
    RN_Renderer   renderer;
    RN_Audio      audio;
    RN_Assets     assets;
    RN_Components components;
} RuinEngine;


void record_command(RuinEngine *engine, VkCommandBuffer cmd, uint32_t img_idx);
void record_command_2d(RuinEngine *engine, VkCommandBuffer cmd, uint32_t img_idx);

void draw_frame(RuinEngine *engine);


void ri_entities_init(RuinEngine *engine);
void ri_entities_kill(RuinEngine *engine);


#ifdef __cplusplus
}
#endif

#endif
