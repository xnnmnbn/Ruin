#include "ruin.h"

#include <stdio.h>
#include <vulkan/vulkan_core.h>


int main(void) {

    RnConfig *cfg = rnConfigGet();

    cfg->window.title      = "Ruin Test";
    cfg->window.width      = 800;
    cfg->window.height     = 450;
    cfg->window.resizable  = RN_TRUE;
    cfg->window.borderless = RN_FALSE;

    cfg->audio.sound_volume = 1.0f;
    cfg->audio.music_volume = 1.0f;

    cfg->renderer.max_frames_in_flight = 2;
    cfg->renderer.resolution_x         = 800;
    cfg->renderer.resolution_y         = 450;
    cfg->renderer.max_anisotropy       = 1.0f;
    cfg->renderer.multisampling        = 1;
    cfg->renderer.vsync                = RN_FALSE;
    

    rnSelfInit(cfg);

    while (rnSelfRunning()) {
    rnFrameBegin();

        if (rnKeyHold(RUIN_KEY_SPACE)) {
            printf("FPS: %d\n", rnTimeFPS());
        }

        if (rnKeyDown(RUIN_KEY_LEFT_CTRL)) {
            cfg->renderer.max_frames_in_flight = 3;
            cfg->window.fullscreen = !cfg->window.fullscreen;
            cfg->window.borderless = !cfg->window.borderless;
            rnConfigUpdatePlatform();
            rnConfigUpdateRenderer();
            printf("ok\n");
        }

    rnFrameEnd();
    }

    rnSelfKill();

    return 0;
}
