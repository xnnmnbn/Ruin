#include "ri_platform.h"
#include "ruin.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_video.h>
#include <stdint.h>
#include <stdio.h>

void ri_platform_window_init(RI_Platform *p) {
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        printf("Failed to init SDL.\n");
        printf("%s\n", SDL_GetError());
        return;
    }

    SDL_WindowFlags flags = SDL_WINDOW_VULKAN;

    RnConfigWindow *cfg = &p->active_config;

    if (cfg->borderless) flags |= SDL_WINDOW_BORDERLESS;
    if (cfg->fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
    if (cfg->resizable)  flags |= SDL_WINDOW_RESIZABLE;
    
    p->window.window = SDL_CreateWindow(cfg->title, cfg->width, cfg->height, flags);

    if (!p->window.window) {
        printf("Failed to create window.\n");
        printf("%s\n", SDL_GetError());
        return;
    }
    
    p->window.running = 1;

    printf("Window created.\n");
}

uint8_t ri_platform_window_running(RI_Platform *p) {
    p->input.mouse_x      = 0.0;
    p->input.mouse_y      = 0.0;
    p->input.mouse_dx     = 0.0;
    p->input.mouse_dy     = 0.0;
    p->input.mouse_scroll = 0.0;
    
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                p->window.running = 0;
                break;

            case SDL_EVENT_MOUSE_WHEEL:
                p->input.mouse_scroll = event.wheel.y;
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                printf("RESIZED\n");
                p->window.resized = 1;
                p->active_config.width  = event.window.data1;
                p->active_config.height = event.window.data2;

            case SDL_EVENT_MOUSE_MOTION:
                p->input.mouse_dx = event.motion.xrel;
                p->input.mouse_dy = event.motion.yrel;
        }
    }

    const bool *keys = SDL_GetKeyboardState(NULL);
    SDL_MouseButtonFlags flags = SDL_GetMouseState(&p->input.mouse_x, &p->input.mouse_y);

    for (size_t i = 0; i< SDL_SCANCODE_COUNT; i++) {
        p->input.keys_was[i] = p->input.keys_now[i];
        p->input.keys_now[i] = keys[i];
    }

    for (size_t i = 0; i < 5; i++) {
        p->input.mouse_was[i] = p->input.mouse_now[i];
        p->input.mouse_now[i] = (uint8_t)((flags >> i) & 1);
    }

    

    return p->window.running;
}


void  ri_platform_window_kill(RI_Platform *p) {
    if (p->window.window) {
        SDL_DestroyWindow(p->window.window);
    }
    p->active_config = (RnConfigWindow){0};
}

