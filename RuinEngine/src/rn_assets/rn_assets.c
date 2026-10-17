#include "rn_assets.h"
#include "rn_renderer/rn_renderer.h"
#include "ruin.h"
#include <stdint.h>
#include <stdio.h>

#include "clists.h"

uint32_t rn_assetstorage_idx(RN_AssetStorage *s) {
    if (s->free_indices.data_count > 0) {
        return *(uint32_t*)fn_dyn_heap_last(&s->free_indices);
    }

    if (s->valid_indices.data_count > 0){
        return s->valid_indices.data_count + 1;
    }

    return 1;
}

void rn_assetstorage_update(RN_AssetStorage *s) {
    uint32_t next_idx = rn_assetstorage_idx(s);

    fn_dyn_heap_push(&s->valid_indices, &next_idx);
}

void rn_assetstorage_kill(RN_AssetStorage *s, uint32_t idx) {
    fn_dyn_heap_remove(&s->valid_indices, &idx, 1);
    fn_dyn_heap_push(&s->free_indices, &idx);
}



void rn_assets_init(RN_Assets *a) {
    rn_assets_audio_init(a);
    rn_assets_texture_init(a);
    RUIN_DEBUG("Assets initialized.\n");
}

void rn_assets_kill(RN_Assets *a, RN_Renderer *r) {
    rn_assets_audio_kill(a);
    rn_assets_texture_kill(a, r);
    RUIN_DEBUG("Assets killed.\n");
}






