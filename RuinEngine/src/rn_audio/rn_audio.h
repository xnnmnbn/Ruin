#include "miniaudio.h"

typedef struct {
    ma_engine engine;
} RN_Audio;

void rn_audio_init(RN_Audio *a);
void rn_audio_kill(RN_Audio *a, void *ass);
