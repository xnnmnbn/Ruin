#include "cvec.h"
#include "ri_components.h"

#include "ruin.h"
#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm/cglm.h>
#include <stddef.h>
#include <stdlib.h>


void ri_components_camera_init(RI_Components *c) {
    c->camera2ds.sparse_data = malloc(RUIN_MAX_ENTITIES * sizeof(RnCamera2D));
    c->i_camera2ds           = malloc(RUIN_MAX_ENTITIES * sizeof(RI_Component_Camera));

    c->camera3ds.sparse_data = malloc(RUIN_MAX_ENTITIES * sizeof(RnCamera3D));
    c->i_camera3ds           = malloc(RUIN_MAX_ENTITIES * sizeof(RI_Component_Camera));
    
    cvec_init(c->camera2ds.dense_indices);
    cvec_init(c->camera3ds.dense_indices);
}


void ri_components_camera_kill(RI_Components *c) {
    free(c->camera2ds.sparse_data);
    free(c->i_camera2ds);
    
    free(c->camera3ds.sparse_data);
    free(c->i_camera3ds);

    cvec_kill(c->camera2ds.dense_indices);
    cvec_kill(c->camera3ds.dense_indices);
}


void ri_components_camera_update(RI_Components *c) {
    for (size_t i = 0; i < c->camera2ds.dense_indices.len; i++) {
        RnEntity e = from_void(c->camera2ds.dense_indices.data, RnEntity)[i];
        RnCamera2D *cmr = &(from_void(c->camera2ds.sparse_data, RnCamera2D)[e]);

        glm_ortho(-1, 1, 1, -1, cmr->near, cmr->far, c->i_camera2ds[e].proj);
        c->i_camera2ds[e].proj[1][1] = -c->i_camera2ds[e].proj[1][1];
        glm_mat4_inv(c->i_transforms.world_matrices[e], c->i_camera2ds[e].view);
    }

    for (size_t i = 0; i < c->camera3ds.dense_indices.len; i++) {
        RnEntity e = from_void(c->camera3ds.dense_indices.data, RnEntity)[i];
        RnCamera3D *cmr = &(from_void(c->camera3ds.sparse_data, RnCamera3D)[e]);

        glm_perspective(cmr->fov, cmr->width / cmr->height, cmr->near, cmr->far, c->i_camera3ds[e].proj);
        glm_mat4_inv(c->i_transforms.world_matrices[e], c->i_camera3ds[e].view);
    }
}
