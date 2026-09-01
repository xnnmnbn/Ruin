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
    cfg->platform.fullscreen = 1; // 0 or not 0

    cfg->renderer.resolution_x = 1920;
    cfg->renderer.resolution_y = 1080;
    cfg->renderer.max_frames_in_flight = 2;
    cfg->renderer.vsync = 1;

    rnSelfInit(cfg);

    cfg->vsync = 0;

    // Config is reapplied only after you call this:
    rnConfigUpdate();

    // It is just a number.
    RnEntity camera_e = rnEntityCreate();

    // Copy value to "camera_e"th point in fixed heap of RnCamera2Ds.
    rnCamera2DAdd(camera_e, (RnCamera2D){
        .width = 1280,
        .height = 720,
        .far = 10,
        .near = 0.1
    });

    // Same as the camera. Just goes to it's own fixed heap.
    rnTransformAdd(camera_e, rnDefaultTransform());

    // Get pointer from fixed heap.
    RnTransform *camera_t = rnTransformGet(camera_e);
    camera_t->position.z = -5;

    // Explicitly tell.
    rnCamera2DUse(camera_e);

    // It's the number. (index of GPU resource heap)
    RnTexture player_tex = rnTextureLoad("player.png");
    RnEntity player = rnEntityCreate();

    // Get default for simplicity.
    RnMaterial2DInfo def_mat = rnDefaultMaterial2DInfo();

    def_mat.texture = player_tex;

    // Get index from another fixed heap.
    RnMaterial2D player_mat = rnMaterial2DCreate(def_mat);

    rnSpriteRendererAdd(player, (RnSpriteRenderer){
        .material = player_mat,
        .layer = 0,
        .dead = 0
    });

    rnTransformAdd(player, rnDefaultTransform());

    // Get pointer from fixed heap.
    RnTransform *player_t = rnTransformGet(player);

    // Dont even skip this.
    rnTextureCreateGpuResources();

    // You can give an array of textures and their count if you don't wanna send all of them.
    // It loads all of them if parameters are zero.
    rnTextureLoadToGpuOffscreen(NULL, 0);

    // Send to VRAM for GUI objects.
    // Isn't necessary in this example.
    // rnTextureLoadToGpuGui(NULL, 0);

    // It sorts once. Call it only if you mutate RnSpriteRenderers.
    // rnSpriteRendererSortByLayer();

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

    // There is no draw function in public API.
    // If entity has a Sprite/Mesh Renderer and a Transform, it is drawable.
    // Dont forget the camera btw.
    rnFrameEnd();
    }

    // Clears the engine resources.
    rnSelfKill();


    return 0;
}
```







