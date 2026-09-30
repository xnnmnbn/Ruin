#include "rn_components.h"

#include "ruin.h"
#include <stdint.h>
#include <stdio.h>
#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#define CGLM_FORCE_LEFT_HANDED
#include <cglm/cglm.h>
#include <stddef.h>
#include <stdlib.h>







void rn_components_camera_update(RN_Components *c) {
    uint32_t c2_count          = c->camera2ds.elem_counts;
    RnCamera2D *c2s            = c->camera2ds.begin_points[0];
    RN_Component_Camera *c2_is = c->camera2ds.begin_points[1];
    RnEntity *c2_es            = c->camera2ds.begin_points[2];

    RnTransform *raw_transforms = (RnTransform*)(c->transforms.begin_points[0]);

    for (size_t i = 0; i < c2_count; i++) {
        RnEntity e                 = c2_es[i];
        RnCamera2D *cmr            = &c2s[e];
        RN_Component_Camera *c2_i  = &c2_is[e];
        RnTransform *t             = &raw_transforms[e];

        glm_ortho(-cmr->width / 2.0f, cmr->width / 2.0f, cmr->height / 2.0f, -cmr->height / 2.0f, cmr->far, -cmr->near, c2_i->proj);
        c2_i->proj[1][1] = -c2_i->proj[1][1];
        glm_mat4_identity(c2_i->view);
        glm_rotate_z(c2_i->view, glm_rad(-t->rotation.z), c2_i->view);
        glm_translate(c2_i->view, (vec3){-t->position.x, -t->position.y, -t->position.z});
    }
}
