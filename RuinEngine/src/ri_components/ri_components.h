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
} RI_ComponentArena;


typedef struct {
    mat4 local_matrix;
    mat4 world_matrix;
    vec3 world_scale;
} RI_Component_Transform;

typedef struct {
    mat4 proj;
    mat4 view;
} RI_Component_Camera;


typedef struct {
    
    RI_ComponentArena transforms;
    RI_ComponentArena camera2ds;
    RI_ComponentArena camera3ds;
    RI_ComponentArena sprite_renderers;

    RnEntity camera_in_use;
    uint8_t camera_mode;

    cvec entities;
    uint32_t next_entity_id;
} RI_Components;


void ri_components_transform_update(RI_Components *c);

void ri_components_camera_update(RI_Components *c);

void ri_components_init(RI_Components *c);
void ri_components_update(RI_Components *c);
void ri_components_kill(RI_Components *c);



#ifdef __cplusplus
}
#endif


#endif
