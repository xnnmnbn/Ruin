#include <stdio.h>
#include "rn_audio.h"
#include "rn_assets/rn_assets.h"


void rn_audio_init(RN_Audio *a) {
    
    if (ma_engine_init(NULL, &a->engine) != MA_SUCCESS) {
        printf("Failed to init Audio.\n");
    }

    printf("Audio initialized.\n");
}


void rn_audio_kill(RN_Audio *a, void *ass) {
    ma_engine_stop(&a->engine);
    ma_engine_uninit(&a->engine);


    RN_Assets *as = ass;
    free(as->sounds.data);
    free(as->musics.data);

    printf("Audio killed.\n");
}

