#include "cvec.h"
#include "ri_components.h"
#include "ruin.h"
#include <cglm/cglm.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>



/*
 RnTransform *ts = NULL;

int sort_by_parent(const void *p1, const void *p2) {
    RnEntity e1 = *((RnEntity*)p1);
    RnEntity e2 = *((RnEntity*)p2);

    if (ts[e1].parent != 0) return -1;

    return 0;
}
*/

/*
void ri_components_transform_init(RI_Components *c) {
    c->transforms.sparse_data      = malloc(RUIN_MAX_ENTITIES * sizeof(RnTransform));
    c->i_transforms.local_matrices = malloc(RUIN_MAX_ENTITIES * sizeof(mat4));
    c->i_transforms.world_matrices = malloc(RUIN_MAX_ENTITIES * sizeof(mat4));
    c->i_transforms.world_scales   = malloc(RUIN_MAX_ENTITIES * sizeof(vec3));

    cvec_init(c->transforms.dense_indices);

    // ts = c->transforms.sparse_data;
}

void ri_components_transform_kill(RI_Components *c) {
    free(c->i_transforms.local_matrices);
    free(c->i_transforms.world_matrices);
    free(c->i_transforms.world_scales);
    free(c->transforms.sparse_data);
    cvec_kill(c->transforms.dense_indices);
}
*/

void ri_components_transform_update(RI_Components *c) {

    RnTransform *transforms = c->transforms.begin_points[0];
    RI_Component_Transform *i_transforms = c->transforms.begin_points[1];
    RnEntity    *entities   = c->transforms.begin_points[2];
    uint32_t     elem_count = c->transforms.elem_counts;

    // qsort(c->transforms.dense_indices.data, c->transforms.dense_indices.len, sizeof(RnEntity), sort_by_parent);

    for (size_t i = 0; i < elem_count; i++) {
        RnEntity e = entities[i];
        RnTransform *transform = &transforms[e];

        if (transform->dead) continue;

        mat4 *l_mat = &i_transforms[e].local_matrix;
        mat4 *w_mat = &i_transforms[e].world_matrix;

        glm_mat4_identity(*l_mat);
        glm_translate(*l_mat, (float[3]){
            transform->position.x,
            -transform->position.y,
            -transform->position.z
        });

        float
            rad_x = glm_rad(transform->rotation.x),
            rad_y = glm_rad(transform->rotation.y),
            rad_z = glm_rad(transform->rotation.z);

        glm_rotate(*l_mat, rad_x, (float[3]){
            1.0f, 0.0f, 0.0f
        });
        glm_rotate(*l_mat, rad_y, (float[3]){
            0.0f, 1.0f, 0.0f
        });
        glm_rotate(*l_mat, rad_z, (float[3]){
            0.0f, 0.0f, 1.0f
        });

        glm_scale(*l_mat, (float[3]){
            transform->scale.x,
            transform->scale.y,
            transform->scale.z
        });

        if (transform->parent == 0) {
            glm_mat4_copy(*l_mat, *w_mat);
            glm_vec3_copy((vec3){
                transform->scale.x,
                transform->scale.y,
                transform->scale.z
            }, i_transforms[e].world_scale);
            continue;
        }

        RnEntity p = transforms[e].parent;
        RnTransform *p_transform = &transforms[p];
        RI_Component_Transform *i_p_transform = &i_transforms[p];
        mat4 *p_mat = &i_p_transform->world_matrix;

        glm_mat4_mul(*p_mat, *l_mat, *w_mat);

        i_transforms[e].world_scale[0] = transform->scale.x * i_p_transform->world_scale[0];
        i_transforms[e].world_scale[1] = transform->scale.y * i_p_transform->world_scale[1];
        i_transforms[e].world_scale[2] = transform->scale.z * i_p_transform->world_scale[2];
    }
}


/*
#include "ri_components.h"
#include <cglm/cglm.h>

// Helper function to resolve a specific entity's matrices down the chain
static void resolve_transform(RI_Components *c, RnEntity e, RnTransform *transforms, bool *computed_this_frame) {
    // If this entity was already processed this frame (either naturally or forced by a child), skip it
    if (computed_this_frame[e]) return;

    RnEntity p = transforms[e].parent;

    // 1. GUARANTEE PARENT IS UPDATED FIRST (Top-Down execution)
    // If there is a parent and it hasn't been processed yet, force resolve it right now!
    if (p != 0 && !computed_this_frame[p]) {
        resolve_transform(c, p, transforms, computed_this_frame);
    }

    // 2. CHECK DIRTY CHAINS
    // A child MUST recalculate if its own local data changed, OR if its parent just changed
    bool parent_changed = (p != 0) && transforms[p].dirty; 
    
    if (!transforms[e].dirty && !parent_changed) {
        computed_this_frame[e] = true;
        return;
    }

    // 3. BUILD LOCAL MATRIX
    mat4 *l_mat = &(c->i_transforms.local_matrices[e]);
    mat4 *w_mat = &(c->i_transforms.world_matrices[e]);

    glm_mat4_identity(*l_mat);
    glm_translate(*l_mat, (float[3]){ transforms[e].position.x, transforms[e].position.y, transforms[e].position.z });

    // Build absolute Euler rotation cleanly
    mat4 rot_mat;
    vec3 rads = {
        glm_rad(transforms[e].rotation.x),
        glm_rad(transforms[e].rotation.y),
        glm_rad(transforms[e].rotation.z)
    };
    glm_euler_zyx(rads, rot_mat); 
    glm_mat4_mul(*l_mat, rot_mat, *l_mat);

    glm_scale(*l_mat, (float[3]){ transforms[e].scale.x, transforms[e].scale.y, transforms[e].scale.z });

    // 4. COMBINE AND PROPAGATE WORLD VALUES
    if (p == 0) { // Root object
        glm_mat4_copy(*l_mat, *w_mat);
        c->i_transforms.world_scales[e][0] = transforms[e].scale.x;
        c->i_transforms.world_scales[e][1] = transforms[e].scale.y;
        c->i_transforms.world_scales[e][2] = transforms[e].scale.z;
    } else { // Child object (Safe because parent is guaranteed to be resolved now)
        glm_mat4_mul(c->i_transforms.world_matrices[p], *l_mat, *w_mat);
        
        c->i_transforms.world_scales[e][0] = transforms[e].scale.x * c->i_transforms.world_scales[p][0];
        c->i_transforms.world_scales[e][1] = transforms[e].scale.y * c->i_transforms.world_scales[p][1];
        c->i_transforms.world_scales[e][2] = transforms[e].scale.z * c->i_transforms.world_scales[p][2];
    }

    // Mark as processed
    computed_this_frame[e] = true;
}

void ri_components_transform_update(RI_Components *c) {
    RnEntity    *entities   = c->transforms.dense_indices.data;
    RnTransform *transforms = c->transforms.sparse_data;
    size_t count            = c->transforms.dense_indices.len;

    if (count == 0) return;

    // Use a fast flat array on the stack to keep track of what was solved this pass
    // For 20,000 entities, a bool array takes practically zero memory.
    bool *computed_this_frame = calloc(RUIN_MAX_ENTITIES, sizeof(bool));

    // Linear pass: Resolve transforms seamlessly
    for (size_t i = 0; i < count; i++) {
        resolve_transform(c, entities[i], transforms, computed_this_frame);
    }

    // Clear internal dirty states once the whole pipeline is accurate
    for (size_t i = 0; i < count; i++) {
        transforms[entities[i]].dirty = false;
    }

    free(computed_this_frame);
}

*/
