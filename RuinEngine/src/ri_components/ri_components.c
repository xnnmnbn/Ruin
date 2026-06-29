#include "ri_components/ri_components.h"
#include "cvec.h"
#include "ruin.h"
#include <stdio.h>
#include <stdlib.h>

void ri_components_init(RI_Components *c) {

    *c = (RI_Components){0};

    cvec_init(c->entities);
    c->next_entity_id = 1;
    
    ri_components_transform_init(c);
    ri_components_camera_init(c);

    c->sprite_renderers.sparse_data = malloc(RUIN_MAX_ENTITIES * sizeof(RnSpriteRenderer));
    cvec_init(c->sprite_renderers.dense_indices);
}



void ri_components_kill(RI_Components *c) {
    ri_components_transform_kill(c);
    ri_components_camera_kill(c);

    free(c->sprite_renderers.sparse_data);
    cvec_kill(c->sprite_renderers.dense_indices);

    printf("Components killed.\n");
}



void ri_components_update(RI_Components *c) {
    ri_components_transform_update(c);
    ri_components_camera_update(c);
}




