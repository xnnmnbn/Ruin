#include "clists.h"
#include "miniaudio.h"
#include "rn_assets.h"
#include "ruin.h"
#include <stdint.h>
#include <stdlib.h>

void rn_assets_audio_init(RN_Assets *a) {
    a->sounds.data = fn_fixed_heap_new(sizeof(RN_Asset_Sound), RUIN_MAX_SOUNDS);
    a->musics.data = fn_fixed_heap_new(sizeof(ma_sound), RUIN_MAX_MUSICS);

    a->sounds.valid_indices = fn_dyn_heap_new(sizeof(RnSound));
    a->sounds.free_indices  = fn_dyn_heap_new(sizeof(RnSound));
    a->musics.valid_indices = fn_dyn_heap_new(sizeof(RnSound));
    a->sounds.free_indices  = fn_dyn_heap_new(sizeof(RnSound));
}

void rn_assets_audio_kill(RN_Assets *a) {

    for (uint32_t i = 0; i < a->sounds.valid_indices.data_count; i++) {
        
        RnSound *idx = fn_dyn_heap_at(&a->sounds.valid_indices, i);
        RN_Asset_Sound *s = fn_fixed_heap_at(&a->sounds.data, idx->id);

        for (uint8_t j = 0; j < MAX_SOUND_REF_COUNT; j++) {
            if (ma_sound_is_playing(&s->sounds[j])) {
                ma_sound_stop(&(s->sounds[j]));
            }
        
            ma_sound_uninit(&(s->sounds[j]));
        }
    }


    for (uint32_t i = 0; i < a->musics.valid_indices.data_count; i++) {

        RnMusic *idx = fn_dyn_heap_at(&a->musics.valid_indices, i);

        ma_sound *m = fn_fixed_heap_at(&a->musics.data, idx->id);

        if (ma_sound_is_playing(m)) {
            ma_sound_stop(m);
        }
        
        ma_sound_uninit(m);
    }

    fn_dyn_heap_kill(&a->sounds.valid_indices);
    fn_dyn_heap_kill(&a->sounds.free_indices);
    
    fn_dyn_heap_kill(&a->musics.valid_indices);
    fn_dyn_heap_kill(&a->musics.free_indices);

}
