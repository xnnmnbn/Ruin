#include "rn_components.h"
#include "ruin.h"
#include <cglm/cglm.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>




void rn_components_transform_update(RN_Components *c) {

    RnTransform *transforms = c->transforms.begin_points[0];
    RN_Component_Transform *i_transforms = c->transforms.begin_points[1];
    RnEntity    *entities   = c->transforms.begin_points[2];
    uint32_t     elem_count = c->transforms.elem_counts;

    for (size_t i = 0; i < elem_count; i++) {
        uint32_t e = entities[i].id;
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

        if (transform->parent.id == 0) {
            glm_mat4_copy(*l_mat, *w_mat);
            glm_vec3_copy((vec3){
                transform->scale.x,
                transform->scale.y,
                transform->scale.z
            }, i_transforms[e].world_scale);
            continue;
        }

        uint32_t p = transforms[e].parent.id;
        RnTransform *p_transform = &transforms[p];
        RN_Component_Transform *i_p_transform = &i_transforms[p];
        mat4 *p_mat = &i_p_transform->world_matrix;

        glm_mat4_mul(*p_mat, *l_mat, *w_mat);

        i_transforms[e].world_scale[0] = transform->scale.x * i_p_transform->world_scale[0];
        i_transforms[e].world_scale[1] = transform->scale.y * i_p_transform->world_scale[1];
        i_transforms[e].world_scale[2] = transform->scale.z * i_p_transform->world_scale[2];
    }
}
