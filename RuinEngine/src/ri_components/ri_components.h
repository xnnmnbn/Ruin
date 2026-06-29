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
    void *sparse_data;
    cvec  dense_indices;
} RI_ComponentStorage;


typedef struct {
    mat4 *world_matrices;
    mat4 *local_matrices;
    vec3 *world_scales;
} RI_Component_Transorm;

typedef struct {
    mat4 proj;
    mat4 view;
} RI_Component_Camera;


typedef struct {
    RI_Component_Transorm i_transforms;
    RI_Component_Camera  *i_camera2ds;
    RI_Component_Camera  *i_camera3ds;

    
    RI_ComponentStorage transforms;
    RI_ComponentStorage camera2ds;
    RI_ComponentStorage camera3ds;
    RI_ComponentStorage sprite_renderers;

    RnEntity camera_in_use;
    uint8_t camera_mode;

    cvec entities;
    uint32_t next_entity_id;
} RI_Components;


void ri_components_transform_init(RI_Components *c);
void ri_components_transform_update(RI_Components *c);
void ri_components_transform_kill(RI_Components *c);

void ri_components_camera_init(RI_Components *c);
void ri_components_camera_update(RI_Components *c);
void ri_components_camera_kill(RI_Components *c);

void ri_components_init(RI_Components *c);
void ri_components_update(RI_Components *c);
void ri_components_kill(RI_Components *c);



#ifdef __cplusplus
}
#endif


#endif
