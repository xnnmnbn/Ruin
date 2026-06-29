#include "ruin.h"
#include <stdint.h>
#include <stdio.h>


typedef struct {
    RnEntity cam;
    RnEntity e1;
    RnEntity e2;
} MyGameEntities;

typedef struct {
    RnTransform *t_cam;
    RnTransform *t_e1;
    RnTransform *t_e2;
} MyGameTransforms;

typedef struct {
    RnMaterial2D mat1;
    RnMaterial2D mat2;

    RnMaterial2DInfo *mi1;
    RnMaterial2DInfo *mi2;
} MyGameMaterial2Ds;

typedef struct {
    RnSpriteRenderer *r_e1;
    RnSpriteRenderer *r_e2;
} MyGameSpriteRenderers;


typedef struct {
    MyGameEntities        entities;
    MyGameTransforms      transforms;
    MyGameMaterial2Ds     material2Ds;
    MyGameSpriteRenderers renderers;

    uint8_t c_mod;
} MyGame;



void myGameInit(MyGame *g) {
    g->entities.cam = rnEntityCreate();
    g->entities.e1 = rnEntityCreate();
    g->entities.e2 = rnEntityCreate();

    RnMaterial2DInfo m = rnDefaultMaterial2DInfo();
    
    m.texture = rnTextureCreate("assets/trollface.png");
    g->material2Ds.mat1 = rnMaterial2DCreate(m);

    printf("Mat: %d.\n", g->material2Ds.mat1);

    m.texture = rnTextureCreate("assets/perfect_blue.jpg");
    g->material2Ds.mat2 = rnMaterial2DCreate(m);

    // rnEntityTransformAdd(g->entities.cam2d, rnDefaultTransform());
    rnEntityTransformAdd(g->entities.cam, rnDefaultTransform());
    rnEntityTransformAdd(g->entities.e1, rnDefaultTransform());
    rnEntityTransformAdd(g->entities.e2, rnDefaultTransform());

    g->material2Ds.mi1 = rnMaterial2DGet(g->material2Ds.mat1);
    g->material2Ds.mi2 = rnMaterial2DGet(g->material2Ds.mat2);

    g->transforms.t_cam = rnEntityTransformGet(g->entities.cam);
    
    g->transforms.t_e1 = rnEntityTransformGet(g->entities.e1);
    g->transforms.t_e2 = rnEntityTransformGet(g->entities.e2);

    rnEntitySpriteRendererAdd(g->entities.e1, (RnSpriteRenderer){
        .alive = RN_TRUE,
        .material = g->material2Ds.mat1
    });


    rnEntitySpriteRendererAdd(g->entities.e2, (RnSpriteRenderer){
        .alive = RN_TRUE,
        .material = g->material2Ds.mat2
    });


    g->renderers.r_e1 = rnEntitySpriteRendererGet(g->entities.e1);
    g->renderers.r_e2 = rnEntitySpriteRendererGet(g->entities.e2);

    rnEntityCamera2DAdd(g->entities.cam, (RnCamera2D){
        .width = 1280,
        .height = 720,
        .far = 100.0f,
        .near = 0.1f,
    });

    rnEntityCamera3DAdd(g->entities.cam, (RnCamera3D){
        .width = 1280,
        .height = 720,
        .far = 100,
        .near = 0.1f,
        .fov = 45
    });

    rnEntityCamera3DUse(g->entities.cam);

    rnTextureCreateGPUResources();
    rnTextureLoadAllToGPU();

    g->transforms.t_cam->position.z = -5;
    g->transforms.t_e1->position.x = 1;
    g->transforms.t_e2->position.x = -1;
}




int main(void) {

    RnConfig *cfg = rnConfigGet();

    cfg->window.title      = "Ruin Test";
    cfg->window.width      = 1280;
    cfg->window.height     = 720;
    cfg->window.resizable  = RN_TRUE;
    cfg->window.borderless = RN_FALSE;

    cfg->audio.sound_volume = 1.0f;
    cfg->audio.music_volume = 1.0f;

    cfg->renderer.max_frames_in_flight = 2;
    cfg->renderer.resolution_x         = 1280;
    cfg->renderer.resolution_y         = 720;
    cfg->renderer.max_anisotropy       = 1.0f;
    cfg->renderer.multisampling        = 1;
    cfg->renderer.vsync                = RN_FALSE;
    

    rnSelfInit(cfg);

    MyGame game = {0};

    myGameInit(&game);

    while (rnSelfRunning()) {
    rnFrameBegin();

    if (rnKeyHold(RUIN_KEY_SPACE)) {
        printf("FPS: %d\n", rnTimeFPS());
    }

    if (rnKeyDown(RUIN_KEY_LEFT_CTRL)) {
        cfg->renderer.max_frames_in_flight = 3;
        cfg->window.fullscreen = !cfg->window.fullscreen;
        cfg->window.borderless = !cfg->window.borderless;
        rnConfigUpdatePlatform();
        rnConfigUpdateRenderer();
        printf("ok\n");

        game.renderers.r_e1->material = game.material2Ds.mat2;
    }

    if (rnKeyHold(RUIN_KEY_W)) {
        game.transforms.t_cam->position.z += 5 * rnTimeDelta();
        // game.transforms.t_e1->position.x += 1 * rnTimeDelta();

        //printf("%f.\n", game.transforms.t_cam2d->position.z);
    }

    if (rnKeyHold(RUIN_KEY_S)) {
    game.transforms.t_cam->position.z -= 5 * rnTimeDelta();

        // printf("%f.\n", game.transforms.t_cam2d->position.z);
    }

    RnVec2 mv = rnMouseMove();

    float m_speed = 5;


    game.transforms.t_cam->rotation.x += mv.y * rnTimeDelta() * m_speed;
    game.transforms.t_cam->rotation.y -= mv.x * rnTimeDelta() * m_speed;

    if (rnKeyDown(RUIN_KEY_SPACE)) {
        game.material2Ds.mi1->tint.a -= rnTimeDelta() * 0.2;
        rnEntityCamera2DUse(game.entities.cam);
    }

    rnFrameEnd();
    }

    rnSelfKill();

    return 0;
}
