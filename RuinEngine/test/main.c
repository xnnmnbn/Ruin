#include "ruin.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>


typedef struct {
    RnEntity          id;
    RnTransform      *transform;
    RnSpriteRenderer *renderer;
} MyGameObject;

typedef struct {
    RnTexture troll;
    RnTexture monster;
    RnTexture bomb;
} MyGameTextures;

typedef struct {
    RnMaterial2D troll;
    RnMaterial2D monster;
    RnMaterial2D bomb;
} MyGameMaterials;




typedef struct {
    RnPostProcess   *post_process;

    MyGameTextures  textures;
    MyGameMaterials material2Ds;
    

    
    MyGameObject troll;
    MyGameObject monster;
    MyGameObject camera;

    MyGameObject bomb_pool[10];
    uint32_t     next_bomb_idx;
    uint32_t     bomb_count;   

    uint8_t c_mod;
} MyGame;

MyGameObject myGameObjectCreate(RnMaterial2D m) {
    MyGameObject o = {0};
    o.id = rnEntityCreate();
    rnTransformAdd(o.id, rnDefaultTransform());
    o.transform = rnTransformGet(o.id);

    if (m) {
        rnSpriteRendererAdd(o.id, (RnSpriteRenderer){
            .dead = 0,
            .layer = 0,
            .material = m
        });

        o.renderer = rnSpriteRendererGet(o.id);
    }

    return o;
}

#define PPU 64


void myGameInit(MyGame *g) {

    g->textures.troll = rnTextureCreate("assets/images/trollface_suit.jpg");
    g->textures.monster = rnTextureCreate("assets/images/trollge_monster.jpg");
    g->textures.bomb = rnTextureCreate("assets/images/bomb.jpg");
    printf("All textures are loaded.\n");

    RnMaterial2DInfo d_mat = rnDefaultMaterial2DInfo();
    
    d_mat.texture = g->textures.troll;
    g->material2Ds.troll = rnMaterial2DCreate(d_mat);
    
    d_mat.texture = g->textures.monster;
    g->material2Ds.monster = rnMaterial2DCreate(d_mat);

    d_mat.texture = g->textures.bomb;
    g->material2Ds.bomb = rnMaterial2DCreate(d_mat);

    g->camera = myGameObjectCreate(0);
    g->troll = myGameObjectCreate(g->material2Ds.troll);
    g->monster = myGameObjectCreate(g->material2Ds.monster);



    RnMaterial2DInfo *mi = rnMaterial2DGet(g->material2Ds.troll);

    g->bomb_count = 10;

    for (uint32_t i = 0; i < g->bomb_count; i++) {
        g->bomb_pool[i] = myGameObjectCreate(g->material2Ds.bomb);
        g->bomb_pool[i].renderer->layer = 1;
        g->bomb_pool[i].transform->position.z = 0.2;
        g->bomb_pool[i].transform->scale = (RnVec3) {
            0.5, 0.5, 1.0
        };
        g->bomb_pool[i].transform->position.x = -1000;
    }
    
    
    rnCamera2DAdd(g->camera.id, (RnCamera2D){
        .width = 1280,
        .height = 720,
        .far = 10,
        .near = 0.1
    });
    
    rnCamera2DUse(g->camera.id);
    
    rnTextureCreateGpuResources();
    rnTextureLoadAllToGPU();
    rnSpriteRendererSortByLayer();

    g->troll.transform->position.x = -8 * PPU;
    g->troll.transform->scale.x = 0.5;
    g->troll.transform->scale.y = 0.5;


    g->monster.transform->position.x = 8 * PPU;
    g->monster.transform->scale.x = 0.4;
    g->monster.transform->scale.y = 0.4;

    
    rnTimeSetTargetFPS(60);
}




int main(void) {

    RnConfig *cfg = rnConfigGet();

    RnBool cursor_hidden = 0;

    cfg->window.title      = "Ruin Test";
    cfg->window.width      = 1280;
    cfg->window.height     = 720;
    cfg->window.resizable  = RN_TRUE;
    cfg->window.borderless = RN_FALSE;

    cfg->audio.sound_volume = 1.0f;
    cfg->audio.music_volume = 1.0f;

    cfg->renderer.max_frames_in_flight = 2;
    cfg->renderer.resolution_x         = 1920;
    cfg->renderer.resolution_y         = 1080;
    cfg->renderer.max_anisotropy       = 1.0;
    cfg->renderer.multisampling        = 1;
    cfg->renderer.vsync                = RN_FALSE;
    

    rnSelfInit(cfg);

    MyGame g = {0};

    myGameInit(&g);

    MyGameObject *bombs = g.bomb_pool;
    RnTransform *t_trn = g.troll.transform;
    RnTransform *m_trn = g.monster.transform;

    RnBool thrown = 0;
    RnBool win = 0;

    RnMaterial2DInfo *m_info = rnMaterial2DGet(g.material2Ds.monster);

    while (rnSelfRunning()) {
    rnFrameBegin();


        if (rnKeyHold(RN_KEY_Q)) {
            m_info->bloom += rnTimeDelta() * 100;
        }

        if (rnKeyDown(RN_KEY_F12)) {
            cfg->window.fullscreen = !cfg->window.fullscreen;
            rnConfigUpdatePlatform();
        }

        const float ms = 0.1;

        if (rnKeyHold(RN_KEY_M)) {
            m_info->glitch += ms * rnTimeDelta();
        }

        if (rnKeyHold(RN_KEY_N)) {
            m_info->glitch -= ms * rnTimeDelta();
        }


        rnTimeSetSpeed(1);
        if (rnKeyHold(RN_KEY_SPACE)) {
            rnTimeSetSpeed(0.5);
        }

        if (rnKeyDown(RN_KEY_V)) {
            cfg->renderer.vsync = !cfg->renderer.vsync;
            rnConfigUpdateRenderer();
        }

        RnVec2 mv = rnMouseMove();

        if (rnMouseHold(RN_MOUSE_RIGHT) && !thrown) {
            bombs[g.next_bomb_idx].transform->position = (RnVec3) {
                .x = t_trn->position.x + 2 * PPU,
                .y = bombs[g.next_bomb_idx].transform->position.y + mv.y * 30 * rnTimeDelta(),
                0
            };

            // printf("Move Y: %f\n", mv.y);

            if (rnMouseDown(RN_MOUSE_LEFT)) {
                thrown = 1;
            }
        }

            
        if (thrown) {
            g.bomb_pool[g.next_bomb_idx].transform->position.x += 1000 * rnTimeDelta();
            g.bomb_pool[g.next_bomb_idx].transform->rotation.z += 60  * rnTimeDelta();

            RnVec3 dist = rnVec3Sub(&g.bomb_pool[g.next_bomb_idx].transform->position, &g.monster.transform->position);        
            float mag = rnVec3Mag(&dist);

            m_info->chromatic_aberration += mag * rnTimeDelta() * 0.01;
            m_info->glitch += mag * rnTimeDelta() * 0.002;
            
            if (mag <= 100) {
                win = 1;
                g.next_bomb_idx += 1;
                thrown = 0;
            }

            if (g.bomb_pool[g.next_bomb_idx].transform->position.x >= 10 * PPU) {
                rnTransformKill(g.bomb_pool[g.next_bomb_idx].id);
                rnSpriteRendererKill(g.bomb_pool[g.next_bomb_idx].id);
                g.next_bomb_idx += 1;
                thrown = 0;
            }

            if (rnKeyDown(RN_KEY_P)) {
                g.troll.transform->parent = g.bomb_pool[g.next_bomb_idx].id;

            }
        } else {
            m_info->chromatic_aberration = 0;
            m_info->glitch = 0;
        }

        if (!win) {
            m_trn->position.y += cosf(rnTimeElapsed()) * rnTimeDelta() * 300;
        } else {
            m_trn->rotation.z += 360 * rnTimeDelta();
        }

    rnFrameEnd();
    
    }

    rnSelfKill();

    return 0;
}
