#include "ri_assets/ri_assets.h"
#include "ri_renderer/ri_renderer.h"
#include <stdio.h>

void ri_assets_init(RI_Assets *a) {
    ri_assets_material_init(a);
    ri_assets_texture_init(a);
}

void ri_assets_kill(RI_Assets *a, RI_Renderer *r) {
    ri_assets_material_kill(a);
    ri_assets_texture_kill(a, r);

    printf("Assets killed.\n");
}






