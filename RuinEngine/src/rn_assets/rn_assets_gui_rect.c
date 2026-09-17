#include "cvec.h"
#include "rn_assets.h"
#include "ruin.h"
#include <stdlib.h>

void rn_assets_gui_rect_init(RN_Assets *a) {
    cvec_init(a->gui_rects.free_indices);
    cvec_init(a->gui_rects.valid_indices);
    a->gui_rects.data = malloc(sizeof(RnGuiRectInfo) * RUIN_MAX_GUI_RECTS);
}

void rn_assets_gui_rect_kill(RN_Assets *a) {
    cvec_kill(a->gui_rects.free_indices);
    cvec_kill(a->gui_rects.valid_indices);
    free(a->gui_rects.data);
}
