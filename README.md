# Ruin

This is a game development library written in C.
I use SDL3, Vulkan and OpenAL Soft in three main internal layers in this library.

Platform (window, input and time) layer is mostly done.
Renderer layer is still on development.
I aim to use bindless-rendering method for performance and probably easier development.

It has a partial ECS support only for provided components such as RnTransform, RnSpriteRenderer, etc...
They are automatically updated on rnFrameEnd().

We are still using the test pipeline which only draws a triangle.

```C
#include "ruin.h"

int main() {

    RnConfig *cfg = rnConfigGet();
    // configure through cfg

    rnSelfInit(cfg); // Cannot init if NULL.

    RnEntity player = rnEntityCreate();

    rnEntityTransformAdd(player, rnDefaultTransform());

    RnTransform *trn = rnEntityTransformGet(player);

    while (rnSelfRunning) {
    rnFrameBegin();

        if (rnKeyHold(RUIN_KEY_D)) {
            trn->position.x += 10 * rnTimeDelta();
        }

    rnFrameEnd();
    }

    rnSelfKill();

    return 0;
}
```