# Ruin

This is a game development library written in C.
I use SDL3, Vulkan and OpenAL Soft in three main internal layers in this library.

Platform (window, input and time) layers are mostly done.
Renderer layer is still on development.
I aim to use bindless-rendering method for performance and probably easier development.

It has a partial ECS support only for provided components such as RnTransform, RnSpriteRenderer, etc...
They are automatically updated on rnFrameEnd().

Here is a small example.

```C
#include "ruin.h"

int main() {

    RnConfig *cfg = rnConfigGet();
    // configure through cfg

    rnSelfInit(cfg); // Cannot init if NULL.

    RnEntity e = rnCreateEntity();

    rnEntityTransformAdd(e, rnDefaultTransform());

    RnTransform *t = rnEntityTransformGet(e);

    while (rnSelfRunning) {
    rnFrameBegin();

    if (rnKeyHold(RUIN_KEY_D)) {
        t->position.x += 10 * rnTimeDelta();
    }

    rnFrameEnd();
    }

    rnSelfKill();

    return 0;
}
```