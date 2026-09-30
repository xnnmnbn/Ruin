#include "rn_components.h"
#include "cvec.h"
#include "ruin.h"
#include <stdalign.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ALLOC_COMPONENT_BLOCK(component_struct, data_type, index) \
    do { \
        size_t align = alignof(data_type); \
        begin_point = (offset + align - 1) & ~(align - 1); \
        component_struct.begin_points[index] = ((uint8_t*)component_struct.linear_data) + begin_point; \
        offset = begin_point + RUIN_MAX_ENTITIES * sizeof(data_type); \
    } while(0)


void rn_components_init(RN_Components *c) {
    *c = (RN_Components){0};
    cvec_init(c->entities);
    c->next_entity_id = 1;

    c->transforms.linear_data = malloc(
        RUIN_MAX_ENTITIES * (
            sizeof(RnTransform)           +
            sizeof(RN_Component_Transform) +
            sizeof(RnEntity)
        ) + 100
    );

    c->camera2ds.linear_data = malloc(
        RUIN_MAX_ENTITIES * (
            sizeof(RnCamera2D)          +
            sizeof(RN_Component_Camera) +
            sizeof(RnEntity)
        ) + 100
    );


    c->renderer2ds.linear_data = malloc(
        RUIN_MAX_ENTITIES * (
            sizeof(RnRenderer2D) +
            sizeof(RnEntity)
        ) + 100
    );

    cvec_init(c->transforms.free_entities);
    cvec_init(c->camera2ds.free_entities);
    cvec_init(c->renderer2ds.free_entities);

    c->transforms.data_sizes[0] = sizeof(RnTransform);
    c->transforms.data_sizes[1] = sizeof(RN_Component_Transform);
    c->transforms.data_sizes[2] = sizeof(RnEntity);

    c->camera2ds.data_sizes[0] = sizeof(RnCamera2D);
    c->camera2ds.data_sizes[1] = sizeof(RN_Component_Camera);
    c->camera2ds.data_sizes[2] = sizeof(RnEntity);

    c->renderer2ds.data_sizes[0] = sizeof(RnRenderer2D);
    c->renderer2ds.data_sizes[1] = sizeof(RnEntity);


    size_t offset;
    size_t align;
    uint8_t *base;

    // RnTransform
    base = (uint8_t*)c->transforms.linear_data;
    offset = 0;

    // Block 0: RnTransform (Aligned right at the start)
    align = alignof(RnTransform);
    offset = (offset + align - 1) & ~(align - 1);
    c->transforms.begin_points[0] = base + offset;
    offset += RUIN_MAX_ENTITIES * sizeof(RnTransform);

    // Block 1: RI_Component_Transform
    align = alignof(RN_Component_Transform);
    offset = (offset + align - 1) & ~(align - 1);
    c->transforms.begin_points[1] = base + offset;
    offset += RUIN_MAX_ENTITIES * sizeof(RN_Component_Transform);

    // Block 2: RnEntity
    align = alignof(RnEntity);
    offset = (offset + align - 1) & ~(align - 1);
    c->transforms.begin_points[2] = base + offset;



    // RnCamera2D
    base = (uint8_t*)c->camera2ds.linear_data;
    offset = 0;

    // Block 0: RnCamera2D
    align = alignof(RnCamera2D);
    offset = (offset + align - 1) & ~(align - 1);
    c->camera2ds.begin_points[0] = base + offset;
    offset += RUIN_MAX_ENTITIES * sizeof(RnCamera2D);

    // Block 1: RI_Component_Camera
    align = alignof(RN_Component_Camera);
    offset = (offset + align - 1) & ~(align - 1);
    c->camera2ds.begin_points[1] = base + offset;
    offset += RUIN_MAX_ENTITIES * sizeof(RN_Component_Camera);

    // Block 2: RnEntity
    align = alignof(RnEntity);
    offset = (offset + align - 1) & ~(align - 1);
    c->camera2ds.begin_points[2] = base + offset;



    // RnRenderer2D
    base = (uint8_t*)c->renderer2ds.linear_data;
    offset = 0;

    // Block 0: RnRenderer2D
    align = alignof(RnRenderer2D);
    offset = (offset + align - 1) & ~(align - 1);
    c->renderer2ds.begin_points[0] = base + offset;
    offset += RUIN_MAX_ENTITIES * sizeof(RnRenderer2D);

    // Block 1: RnEntity
    align = alignof(RnEntity);
    offset = (offset + align - 1) & ~(align - 1);
    c->renderer2ds.begin_points[1] = base + offset;
}






void rn_components_kill(RN_Components *c) {

    free(c->transforms.linear_data);
    free(c->camera2ds.linear_data);
    free(c->renderer2ds.linear_data);

    cvec_kill(c->entities);

    RUIN_DEBUG("Components killed.\n");
}



void rn_components_update(RN_Components *c) {
    rn_components_transform_update(c);
    rn_components_camera_update(c);
}




