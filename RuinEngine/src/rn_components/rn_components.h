#ifndef RI_COMPONENTS_H
#define RI_COMPONENTS_H


#include "cvec.h"
#include "ruin.h"
#include <cglm/types.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#define from_void(p, t) ((t*)(p))

typedef struct {
    void    *linear_data;
    void    *begin_points[5];
    uint32_t data_sizes[5];
    uint32_t elem_counts;
    cvec     free_entities;
} RN_ComponentArena;


typedef struct {
    mat4 local_matrix;
    mat4 world_matrix;
    vec3 world_scale;
} RN_Component_Transform;

typedef struct {
    mat4 view;
    mat4 proj;
} RN_Component_Camera;


typedef struct {
    
    RN_ComponentArena transforms;
    RN_ComponentArena camera2ds;
    RN_ComponentArena renderer2ds;
    RN_ComponentArena gui_rects;

    RnEntity camera_in_use;
    uint8_t camera_mode;

    cvec entities;
    uint32_t next_entity_id;
} RN_Components;


void rn_components_transform_update(RN_Components *c);

void rn_components_camera_update(RN_Components *c);

void rn_components_init(RN_Components *c);
void rn_components_update(RN_Components *c);
void rn_components_kill(RN_Components *c);



#ifdef __cplusplus
}
#endif


#endif
