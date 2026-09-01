#include "cvec.h"
#include "ri_components.h"

#include "ruin.h"
#include <stdint.h>
#include <stdio.h>
#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#define CGLM_FORCE_LEFT_HANDED
#include <cglm/cglm.h>
#include <stddef.h>
#include <stdlib.h>

/*
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
*/



/*
void ri_components_camera_update(RI_Components *c) {
    uint32_t c2_count          = c->camera2ds.elem_counts;
    RnCamera2D *c2s            = c->camera2ds.begin_points[0];
    RI_Component_Camera *c2_is = c->camera2ds.begin_points[1];
    RnEntity *c2_es         = c->camera2ds.begin_points[2];

    uint32_t c3_count          = c->camera3ds.elem_counts;
    RnCamera3D *c3s            = c->camera3ds.begin_points[0];
    RI_Component_Camera *c3_is = c->camera3ds.begin_points[1];
    RnEntity *c3_es            = c->camera3ds.begin_points[2];

    for (size_t i = 0; i < c2_count; i++) {
        RnEntity e                =  c2_es[i];
        RnCamera2D *cmr           = &c2s[e];
        RI_Component_Camera *c2_i = &c2_is[e];
        RI_Component_Transform *t = &(
            (RI_Component_Transform*)(c->transforms.begin_points[1])
        )[e];

        glm_ortho(-cmr->width / 2, cmr->width / 2, -cmr->height / 2, cmr->height / 2, cmr->far, cmr->near, c2_i->proj);
        c2_i->proj[1][1] = -c2_i->proj[1][1];
        glm_mat4_inv(t->world_matrix, c2_i->view);
    }

    for (size_t i = 0; i < c3_count; i++) {
        RnEntity e                =  c3_es[i];
        RnCamera3D *cmr           = &c3s[e];
        RI_Component_Camera *c3_i = &c3_is[e];
        RI_Component_Transform *t = &(
            (RI_Component_Transform*)(c->transforms.begin_points[1])
        )[e];

        glm_perspective(glm_rad(cmr->fov), (float)cmr->width / (float)cmr->height, cmr->near, cmr->far, c3_i->proj);
        c3_i->proj[1][1] = -c3_i->proj[1][1];
        glm_mat4_inv(t->world_matrix, c3_i->view);
    }
    float *f = (float*)&c3_is[c->camera_in_use].proj;

    for (uint32_t i = 0; i < 16; i++) {
        printf("Proj #%d: %f\n",i, f[i]);
    }
}
*/





void ri_components_camera_update(RI_Components *c) {
    uint32_t c2_count          = c->camera2ds.elem_counts;
    RnCamera2D *c2s            = c->camera2ds.begin_points[0];
    RI_Component_Camera *c2_is = c->camera2ds.begin_points[1];
    RnEntity *c2_es            = c->camera2ds.begin_points[2];

    uint32_t c3_count          = c->camera3ds.elem_counts;
    RnCamera3D *c3s            = c->camera3ds.begin_points[0];
    RI_Component_Camera *c3_is = c->camera3ds.begin_points[1];
    RnEntity *c3_es            = c->camera3ds.begin_points[2];

    // Get the sparse base pointer for raw user-facing transforms
    RnTransform *raw_transforms = (RnTransform*)(c->transforms.begin_points[0]);

    for (size_t i = 0; i < c2_count; i++) {
        RnEntity e                 = c2_es[i];
        RnCamera2D *cmr            = &c2s[e];
        RI_Component_Camera *c2_i  = &c2_is[e];
        RnTransform *t             = &raw_transforms[e];

        // 1. Clean Projection Calculation
        glm_ortho(-cmr->width / 2.0f, cmr->width / 2.0f, cmr->height / 2.0f, -cmr->height / 2.0f, cmr->near, -cmr->far, c2_i->proj);
        c2_i->proj[1][1] = -c2_i->proj[1][1]; // Keep your Vulkan/inverted-Y patch

        // 2. FIX: Build a safe view matrix without relying on inverse matrix calculations
        glm_mat4_identity(c2_i->view);
        // Apply inverse transformations manually (Rotate then Translate)
        glm_rotate_z(c2_i->view, glm_rad(-t->rotation.z), c2_i->view);
        glm_translate(c2_i->view, (vec3){-t->position.x, -t->position.y, -t->position.z});
    }

    for (size_t i = 0; i < c3_count; i++) {
        RnEntity e                 = c3_es[i];
        RnCamera3D *cmr            = &c3s[e];
        RI_Component_Camera *c3_i  = &c3_is[e];
        RnTransform *t             = &raw_transforms[e];

        // 1. Clean Perspective Calculation
        float aspect = (cmr->height > 0) ? (float)cmr->width / (float)cmr->height : 16.0f / 9.0f;
        glm_perspective(glm_rad(cmr->fov), aspect, cmr->near, cmr->far, c3_i->proj);
        c3_i->proj[1][1] = -c3_i->proj[1][1];

        // 2. FIX: Standard FPS / Freecam 3D View construction
        glm_mat4_identity(c3_i->view);
        glm_rotate_x(c3_i->view, glm_rad(-t->rotation.x), c3_i->view);
        glm_rotate_y(c3_i->view, glm_rad(t->rotation.y), c3_i->view);
        glm_rotate_z(c3_i->view, glm_rad(t->rotation.z), c3_i->view);
        glm_translate(c3_i->view, (vec3){-t->position.x, -t->position.y, -t->position.z});
    }
}
