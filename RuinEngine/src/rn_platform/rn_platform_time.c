#include "rn_platform.h"

#include <SDL3/SDL_timer.h>

void rn_platform_time_init(RN_Platform *p) {
    p->time = (RN_Time){0};
    p->time.freq = SDL_GetPerformanceFrequency();
    p->time.now  = SDL_GetPerformanceCounter();
    p->time.speed = 1.0;
    p->time.target_delta = 1.0 / 60.0;
}

static uint8_t frame_count = 0;
static float   each_second = 0.0f;
static size_t  sum_fps = 0;

void rn_platform_time_update(RN_Platform *p) {
    p->time.was = p->time.now;
    p->time.now = SDL_GetPerformanceCounter();

    p->time.fixed_delta    = p->time.fixed_delta = ((double)(p->time.now - p->time.was) / (double)p->time.freq);
    p->time.fixed_elapsed += p->time.fixed_delta;

    p->time.delta    = p->time.fixed_delta * p->time.speed;
    p->time.elapsed += p->time.delta;

    each_second += p->time.fixed_delta;
    frame_count += 1;
    sum_fps     += (uint8_t)(1.0 / p->time.fixed_delta);

    if (each_second >= 1.0) {

        p->time.fps = sum_fps / frame_count;

        each_second = 0.0f;
        frame_count = 0;
        sum_fps     = 0;
    }
}


