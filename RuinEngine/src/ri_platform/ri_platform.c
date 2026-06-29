#include "ri_platform.h"
#include "ruin.h"

#include <stdio.h>

void ri_platform_init(RI_Platform *p) {
    ri_platform_window_init(p);
    ri_platform_input_init(p);
    ri_platform_time_init(p);

    printf("Platform initialized.\n");
}

void ri_platform_kill(RI_Platform *p) {
    if (p->window.window != NULL) {
        ri_platform_window_kill(p);
    }

    if (p->input.keys_now != NULL) {
        ri_platform_input_kill(p);
    }

    printf("Platform killed.\n");
}
