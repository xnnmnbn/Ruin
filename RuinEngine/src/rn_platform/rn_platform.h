#ifndef RI_PLATFORM_H
#define RI_PLATFORM_H

#include "ruin.h"
#include <SDL3/SDL_events.h>


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <SDL3/SDL.h>


typedef struct RN_Window {
    SDL_Event   event;
    SDL_Window *window;
    uint8_t     running;
    uint8_t     resized;
} RN_Window;

typedef struct RN_Time {
    float    delta;
    float    fixed_delta;
    float    elapsed;
    float    fixed_elapsed;
    float    speed;
    float    target_delta;
    uint8_t  fps;
    uint64_t freq;
    uint64_t now;
    uint64_t was;
} RN_Time;

typedef struct RN_Input {
    uint8_t *keys_now;
    uint8_t *keys_was;
    uint8_t *mouse_now;
    uint8_t *mouse_was;
    float    mouse_x;
    float    mouse_y;
    float    mouse_dx;
    float    mouse_dy;
    float    mouse_scroll;
} RN_Input;

typedef struct RN_Platform {
    RnConfigWindow config;
    RN_Window      window;
    RN_Time        time;
    RN_Input       input;
} RN_Platform;


void   rn_platform_window_init(RN_Platform *p);
RnBool rn_platform_window_running(RN_Platform *p);
void   rn_platform_window_kill(RN_Platform *p);

void rn_platform_input_init(RN_Platform *p);
void rn_platform_input_update(RN_Platform *p);
void rn_platform_input_kill(RN_Platform *p);

void rn_platform_time_init(RN_Platform *p);
void rn_platform_time_update(RN_Platform *p);

void rn_platform_init(RN_Platform *p);
void rn_platform_kill(RN_Platform *p);

#ifdef __cplusplus
}
#endif

#endif
