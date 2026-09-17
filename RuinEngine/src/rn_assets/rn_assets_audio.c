#include "cvec.h"
#include "miniaudio.h"
#include "rn_assets.h"
#include "ruin.h"
#include <stdint.h>
#include <stdlib.h>

void rn_assets_audio_init(RN_Assets *a) {
    a->sounds.data = malloc(RUIN_MAX_SOUNDS * sizeof(ma_sound));
    a->musics.data = malloc(RUIN_MAX_MUSICS * sizeof(ma_sound));

    cvec_init(a->sounds.valid_indices);
    cvec_init(a->sounds.free_indices);
    cvec_init(a->musics.valid_indices);
    cvec_init(a->sounds.free_indices);
}

void rn_assets_audio_kill(RN_Assets *a) {

    for (uint32_t i = 0; i < a->sounds.valid_indices.len; i++) {
        ma_sound *s = &(
            ((ma_sound*)a->sounds.data)
        )[i];

        if (ma_sound_is_playing(s)) {
            ma_sound_stop(s);
        }
        
        ma_sound_uninit(s);
    }


    for (uint32_t i = 0; i < a->musics.valid_indices.len; i++) {

        ma_sound *m = &(
            ((ma_sound*)a->musics.data)
        )[i];

        if (ma_sound_is_playing(m)) {
            ma_sound_stop(m);
        }
        
        ma_sound_uninit(m);
    }

    cvec_kill(a->sounds.valid_indices);
    cvec_kill(a->sounds.free_indices);
    cvec_kill(a->musics.valid_indices);
    cvec_kill(a->musics.free_indices);

}
