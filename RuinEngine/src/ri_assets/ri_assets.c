#include "ri_assets/ri_assets.h"
#include "cvec.h"
#include "ri_renderer/ri_renderer.h"
#include <stdint.h>
#include <stdio.h>

uint32_t ri_assetstorage_idx(RI_AssetStorage *s) {
    if (s->free_indices.len > 0) {
        return ((uint32_t*)s->free_indices.data)[s->free_indices.len - 1];
    }

    if (s->valid_indices.len > 0){
        return s->valid_indices.len + 1;
    }

    return 1;
}

void ri_assetstorage_update(RI_AssetStorage *s) {
    uint32_t next_idx = ri_assetstorage_idx(s);

    cvec_push(s->valid_indices, next_idx, uint32_t);
}

void ri_assetstorage_kill(RI_AssetStorage *s, uint32_t idx) {
    cvec_remove(s->valid_indices, idx, uint32_t);
    cvec_push(s->free_indices, idx, uint32_t);
}
void ri_assets_init(RI_Assets *a) {
    ri_assets_material_init(a);
    ri_assets_texture_init(a);
    
    // cvec_push(a->gpu_textures.valid_indices, 0, uint32_t);
    cvec_push(a->material2Ds.valid_indices, 0, uint32_t);
    cvec_push(a->material3Ds.valid_indices, 0, uint32_t);
}

void ri_assets_kill(RI_Assets *a, RI_Renderer *r) {
    ri_assets_material_kill(a);
    ri_assets_texture_kill(a, r);

    printf("Assets killed.\n");
}






