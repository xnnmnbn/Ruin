#include "rn_assets.h"
#include "rn_renderer/rn_renderer.h"
#include "cvec.h"
#include <stdio.h>

uint32_t rn_assetstorage_idx(RN_AssetStorage *s) {
    if (s->free_indices.len > 0) {
        return ((uint32_t*)s->free_indices.data)[s->free_indices.len - 1];
    }

    if (s->valid_indices.len > 0){
        return s->valid_indices.len + 1;
    }

    return 1;
}

void rn_assetstorage_update(RN_AssetStorage *s) {
    uint32_t next_idx = rn_assetstorage_idx(s);

    cvec_push(s->valid_indices, next_idx, uint32_t);
}

void rn_assetstorage_kill(RN_AssetStorage *s, uint32_t idx) {
    cvec_remove(s->valid_indices, idx, uint32_t);
    cvec_push(s->free_indices, idx, uint32_t);
}
void rn_assets_init(RN_Assets *a) {
    rn_assets_audio_init(a);
    rn_assets_material_init(a);
    rn_assets_texture_init(a);
    rn_assets_gui_rect_init(a);

    printf("Assets initialized.\n");
}

void rn_assets_kill(RN_Assets *a, RN_Renderer *r) {
    rn_assets_audio_kill(a);
    rn_assets_material_kill(a);
    rn_assets_texture_kill(a, r);
    rn_assets_gui_rect_kill(a);

    printf("Assets killed.\n");
}






