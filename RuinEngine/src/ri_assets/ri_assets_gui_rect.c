#include "cvec.h"
#include "ri_assets/ri_assets.h"
#include "ruin.h"
#include <stdlib.h>

void ri_assets_gui_rect_init(RI_Assets *a) {
    cvec_init(a->gui_rects.free_indices);
    cvec_init(a->gui_rects.valid_indices);
    a->gui_rects.data = malloc(sizeof(RnGuiRectInfo) * RUIN_MAX_GUI_RECTS);
}

void ri_assets_gui_rect_kill(RI_Assets *a) {
    cvec_kill(a->gui_rects.free_indices);
    cvec_kill(a->gui_rects.valid_indices);
    free(a->gui_rects.data);
}
