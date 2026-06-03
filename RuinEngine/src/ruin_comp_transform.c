#include "cvec.h"
#include "ruin.h"
#include "ruin__internal.h"
#include <cglm/affine-pre.h>
#include <cglm/affine.h>
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <cglm/util.h>
#include <cglm/vec3.h>
#include <stdint.h>
#include <stdlib.h>

static RuinInternal *eng_ptr;

#define from_void(V, T) ((T*)V)

int32_t sortByParent(const void *_a, const void *_b) {
    RnEntity a = *((RnEntity*)_a);
    RnEntity b = *((RnEntity*)_b);

    int32_t p_a = (from_void(eng_ptr->components.transforms.sparse_data, RnTransform)[a].parent != 0);
    int32_t p_b = (from_void(eng_ptr->components.transforms.sparse_data, RnTransform)[b].parent != 0);

    return p_b - p_a;
}

void ri_comp_transform_init(RuinInternal *engine) {
    RI_TransformInternal *ti = &engine->component_internals.transform_internal;
    
    ti->dense_matrices_to_gpu  = malloc(1024 * sizeof(mat4));
    ti->sparse_world_matrices  = malloc(RUIN_MAX_ENTITIES * sizeof(mat4));
    ti->sparse_local_matrices  = malloc(RUIN_MAX_ENTITIES * sizeof(mat4));
    ti->sparse_world_positions = malloc(RUIN_MAX_ENTITIES * sizeof(vec3));
    ti->sparse_world_rotations = malloc(RUIN_MAX_ENTITIES * sizeof(vec3));
    ti->sparse_world_scales    = malloc(RUIN_MAX_ENTITIES * sizeof(vec3));
}

void ri_comp_transform_kill(RuinInternal *engine) {
    RI_TransformInternal *ti = &engine->component_internals.transform_internal;

    free(ti->dense_matrices_to_gpu);
    free(ti->sparse_world_matrices);
    free(ti->sparse_local_matrices);
    free(ti->sparse_world_positions);
    free(ti->sparse_world_rotations);
    free(ti->sparse_world_scales);

    cvec_kill(ti->dense_indices_for_cpu);
    cvec_kill(ti->dense_indices_for_gpu);
}

void ri_comp_transform_update(RuinInternal *engine) {
    size_t user_created_transform_count = engine->components.transforms.dense_indices.len;

    qsort(engine->components.transforms.dense_indices.data, engine->components.transforms.dense_indices.len, sizeof(RnEntity), sortByParent);
    
    for (size_t i = 0; i < user_created_transform_count; i++) {
        RnEntity     e  =  cvec_t(engine->components.transforms.dense_indices, RnEntity)[i];
        RnTransform *t  = &void_t(engine->components.transforms.sparse_data, RnTransform)[e];

        if (t->dirty) continue;
        
        mat4 *lm = &(engine->component_internals.transform_internal.sparse_local_matrices[e]);
        mat4 *wm = &(engine->component_internals.transform_internal.sparse_world_matrices[e]);
        vec3 *wp = &(engine->component_internals.transform_internal.sparse_world_positions[e]);
        vec3 *wr = &(engine->component_internals.transform_internal.sparse_world_rotations[e]);
        vec3 *ws = &(engine->component_internals.transform_internal.sparse_world_scales[e]);

        vec3 lp = {
            t->position.x,
            t->position.y,
            t->position.z,
        };
        
        vec3 lr = {
            glm_rad(t->rotation.x),
            glm_rad(t->rotation.y),
            glm_rad(t->rotation.z),
        };
        
        vec3 ls = { t->scale.x, t->scale.z, t->scale.z };

        glm_translate(*lm, lp);
        
        glm_rotate_x(*lm, lr[0], *lm);
        glm_rotate_y(*lm, lr[1], *lm);
        glm_rotate_z(*lm, lr[2], *lm);
        glm_scale(*lm, ls);


        for (size_t s = 0; s < 16; s++) {
            
        }

        if (t->parent == 0) {
            glm_mat4_copy(*lm, *wm);
            
            glm_vec3_copy(lp, *wp);
            glm_vec3_copy(lr, *wr);
            glm_vec3_copy(ls, *ws);
        } else {
            mat4 *pm = &(engine->component_internals.transform_internal.sparse_world_matrices[t->parent]);
            glm_mat4_mul(*pm, *lm, *wm);
            glm_vec3_copy((*wm)[3], *wp);
        }
    }
}






