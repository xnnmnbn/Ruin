#include "cvec.h"
#include "ri_assets/ri_assets.h"
#include "ruin.h"
#include <stdint.h>
#include <stdlib.h>


void ri_assets_material_init(RI_Assets *a) {
    a->material2Ds.data = malloc(RUIN_MAX_MATERIALS * sizeof(RnMaterial2DInfo));
    cvec_init(a->material2Ds.valid_indices);
    a->material3Ds.data = malloc(RUIN_MAX_MATERIALS * sizeof(RnMaterial3DInfo));
    cvec_init(a->material3Ds.valid_indices);
}


void ri_assets_material_kill(RI_Assets *a) {
    free(a->material2Ds.data);
    cvec_kill(a->material2Ds.valid_indices);

    free(a->material3Ds.data);
    cvec_kill(a->material3Ds.valid_indices);
}







