#include "ri_platform/ri_platform.h"
#include "ruin.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void ri_platform_input_init(RI_Platform *p) {
    p->input = (RI_Input){0};
    p->input.keys_now  = (RnBool *)malloc(sizeof(uint8_t) * 512);
    p->input.keys_was  = (RnBool *)malloc(sizeof(uint8_t) * 512);
    p->input.mouse_now = (RnBool *)malloc(sizeof(uint8_t) * 5);
    p->input.mouse_was = (RnBool *)malloc(sizeof(uint8_t) * 5);

    memset(p->input.keys_now,  0, sizeof(uint8_t) * 512);
    memset(p->input.keys_was,  0, sizeof(uint8_t) * 512);
    memset(p->input.mouse_now, 0, sizeof(uint8_t) * 5);
    memset(p->input.mouse_was, 0, sizeof(uint8_t) * 5);
}

void ri_platform_input_kill(RI_Platform *p) {
    free(p->input.keys_now);
    free(p->input.keys_was);
    free(p->input.mouse_now);
    free(p->input.mouse_was);
}
