#include "rn_platform.h"
#include "ruin.h"

#include <stdio.h>

void rn_platform_init(RN_Platform *p) {
    rn_platform_window_init(p);
    rn_platform_input_init(p);
    rn_platform_time_init(p);

    printf("Platform initialized.\n");
}

void rn_platform_kill(RN_Platform *p) {
    if (p->window.window != NULL) {
        rn_platform_window_kill(p);
    }

    if (p->input.keys_now != NULL) {
        rn_platform_input_kill(p);
    }

    printf("Platform killed.\n");
}
