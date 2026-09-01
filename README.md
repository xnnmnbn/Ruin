# Ruin Engine
### For those sick enough to ruin

This is a game development library written in C.
I aim to give the callbacks under the buttons of a game engine while keeping it relatively simple and close to the hardware.
There is no safety checks in this engine. You should use your common sense.

# examples

### Just a window
```C
#include "ruin.h"

int main() {
    RnConfig *cfg = rnConfigGet();

    // other options exist
    cfg->platform.title = "Ruin Test";
    cfg->platform.width = 1280;
    cfg->platform.height = 720;

    // other options exist
    cfg->renderer.resolution_x = 1920;
    cfg->renderer.resolution_y = 1080;

    rnSelfInit(cfg);

    while (rnSelfRunning()) {
    rnFrameBegin();
        // your game
    rnFrameEnd();
    }

    rnSelfKill();

    return 0;
}
```
---
### Walking Simulation
```C
#include "ruin.h"

int main() {
    RnConfig *cfg = rnConfigGet();

    cfg->platform.title = "Ruin Test";
    cfg->platform.width = 1280;
    cfg->platform.height = 720;
    cfg->platform.fullscreen = 1;

    cfg->renderer.resolution_x = 1920;
    cfg->renderer.resolution_y = 1080;
    cfg->renderer.max_frames_in_flight = 2;
    cfg->renderer.vsync = 0;

    rnSelfInit(cfg);

    RnEntity camera_e = rnEntityCreate();
    rnCamera2DAdd(camera_e, (RnCamera2D){
        .width = 1280,
        .height = 720,
        .far = 10,
        .near = 0.1
    });

    rnTransformAdd(camera_e, rnDefaultTransform());
    RnTransform *camera_t = rnTransformGet(camera_e);
    camera_t->position.z = -5;

    rnCamera2DUse(camera_e);

    RnTexture player_tex = rnTextureLoad("player.png");
    RnEntity player = rnEntityCreate();

    RnMaterial2DInfo def_mat = rnDefaultMaterial2DInfo();
    def_mat.texture = player_tex;
    RnMaterial2D player_mat = rnMaterial2DCreate(def_mat);

    rnSpriteRendererAdd(player, (RnSpriteRenderer){
        .material = player_mat,
        .layer = 0,
        .dead = 0
    });

    rnTransformAdd(player, rnDefaultTransform());
    RnTransform *player_t = rnTransformGet(player);

    rnTextureCreateGpuResources();
    rnTextureLoadToGpuOffscreen(NULL, 0);

    // Just know you can call it whenever you want after initialization.
    rnSpriteRendererSortByLayer();

    // Pretty necessary
    rnTimeTargetFPS(169);

    while (rnSelfRunning()) {
    rnFrameBegin();
        player_t->position.x += (
            (rnKeyHold(RN_KEY_A) ? -1 : 0) +
            (rnKeyHold(RN_KEY_D) ?  1 : 0)
        ) * rnTimeDelta() * 200;

        player_t->position.y += (
            (rnKeyHold(RN_KEY_S) ? -1 : 0) +
            (rnKeyHold(RN_KEY_W) ?  1 : 0)
        ) * rnTimeDelta() * 200;
    rnFrameEnd();
    }

    rnSelfKill();
    return 0;
}
```







