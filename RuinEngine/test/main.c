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
    RnPostProcess        *post_process;
    MyGameEntities        entities;
    MyGameTransforms      transforms;
    MyGameMaterial2Ds     material2Ds;
    MyGameSpriteRenderers renderers;

    uint8_t c_mod;
} MyGame;


RnTransform *ts[RUIN_MAX_ENTITIES];


float *data;


void myGameInit(MyGame *g) {

    data = (float[3]){
        31, 31, 31
    };
    
    g->entities.cam = rnEntityCreate();
    g->entities.e1 = rnEntityCreate();
    g->entities.e2 = rnEntityCreate();

    g->post_process = rnPostProcessGet();
   *g->post_process = rnDefaultPostProcess();
    // g->post_process->tint.g = 0.25f;
    // g->post_process->tint.b = 0.25f;


    RnTransform deft = rnDefaultTransform();


    

    



    RnMaterial2DInfo m = rnDefaultMaterial2DInfo();
    
    m.texture = rnTextureCreate("assets/trollface.png");
    g->material2Ds.mat1 = rnMaterial2DCreate(m);    
    m.texture = rnTextureCreate("assets/perfect_blue.jpg");
    g->material2Ds.mat2 = rnMaterial2DCreate(m);

    rnTransformAdd(g->entities.cam, rnDefaultTransform());
    rnTransformAdd(g->entities.e1, rnDefaultTransform());
    rnTransformAdd(g->entities.e2, rnDefaultTransform());
    
    g->material2Ds.mi1 = rnMaterial2DGet(g->material2Ds.mat1);
    g->material2Ds.mi2 = rnMaterial2DGet(g->material2Ds.mat2);

    g->transforms.t_cam = rnTransformGet(g->entities.cam);
    g->transforms.t_e1 = rnTransformGet(g->entities.e1);
    g->transforms.t_e2 = rnTransformGet(g->entities.e2);

    rnSpriteRendererAdd(g->entities.e2, (RnSpriteRenderer){
        .dead = 0,
        .material = g->material2Ds.mat2
    });


    rnSpriteRendererAdd(g->entities.e1, (RnSpriteRenderer){
        .dead = 0,
        .material = g->material2Ds.mat1,
        .layer = 10
    });


    for (uint32_t i = 0; i < 5; i++) {
        RnSpriteRenderer sr = {0};
        sr.material = g->material2Ds.mat1;
        sr.layer = i;
        RnEntity e = rnEntityCreate();
        deft.position.x += 200;
        deft.position.z = 1;
        deft.scale.x += 0.2;
        deft.scale.y += 0.2;
        rnTransformAdd(e, deft);
        rnSpriteRendererAdd(e, sr);
    }


    g->renderers.r_e1 = rnSpriteRendererGet(g->entities.e1);
    g->renderers.r_e2 = rnSpriteRendererGet(g->entities.e2);

    rnCamera2DAdd(g->entities.cam, (RnCamera2D){
        .width = 1280,
        .height = 720,
        .far = 10,
        .near = 0.1,
        .target = 0
    });

    rnCamera3DAdd(g->entities.cam, (RnCamera3D){
        .width = 1280,
        .height = 720,
        .far = 1000.0,
        .near = 0.1,
        .fov = 45,
        .target = 0
    });
    
    rnCamera2DUse(g->entities.cam);

    rnTextureCreateGPUResources();
    rnTextureLoadAllToGPU();

    g->transforms.t_cam->position.z = -1;
    g->transforms.t_e1->position.z = 1;
    g->transforms.t_e2->position.z = 1;
    // g->transforms.t_e1->scale.x = 0.3;
    // g->transforms.t_e1->scale.y = 0.3;
    // g->transforms.t_e2->position.x = 0;

    rnSpriteRendererSortByLayer();
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
    cfg->renderer.resolution_x         = 1280;
    cfg->renderer.resolution_y         = 720;
    cfg->renderer.max_anisotropy       = 1;
    cfg->renderer.multisampling        = 1;
    cfg->renderer.vsync                = RN_FALSE;
    

    rnSelfInit(cfg);

    rnTimeSetTargetFPS(30);

    MyGame game = {0};

    myGameInit(&game);

    printf("%f, %f, %f\n", data[0], data[1], data[2]);

    while (rnSelfRunning()) {
    rnFrameBegin();

    if (rnKeyHold(RUIN_KEY_SPACE)) {
        printf("FPS: %d\n", rnTimeFPS());
    }

    if (rnKeyUp(RUIN_KEY_ESCAPE)) {
        cursor_hidden = !cursor_hidden;
        rnMouseHideCursor(cursor_hidden);
    }

    if (rnKeyDown(RUIN_KEY_LEFT_CTRL)) {
        cfg->window.fullscreen = !cfg->window.fullscreen;
        cfg->window.borderless = !cfg->window.borderless;
        rnConfigUpdatePlatform();
        rnConfigUpdateRenderer();
        printf("ok\n");

        game.renderers.r_e1->material = game.material2Ds.mat1;
    }

    // printf("FPS: %d\n", rnTimeFPS());

    
    if (rnKeyHold(RUIN_KEY_W)) {

        // rnEntityCamera2DGet(game.entities.cam)->height -= rnTimeDelta() * 40;
        game.post_process->contrast -= rnTimeDelta() * 0.5;

        // game.transforms.t_cam->position.x += 200 * rnTimeDelta();
        // game.transforms.t_e1->scale.x += rnTimeDelta();
        game.transforms.t_cam->position.z += rnTimeDelta();
    }

    if (rnKeyHold(RUIN_KEY_S)) {

        game.transforms.t_e1->rotation.z -= 10 * rnTimeDelta();

        game.post_process->contrast += rnTimeDelta() * 0.5;
    }

    RnVec2 mv = rnMouseMove();

    float m_speed = 50;
    // game.transforms.t_cam->rotation.x += mv.y * rnTimeDelta() * m_speed;
    // game.transforms.t_cam->rotation.y -= mv.x * rnTimeDelta() * m_speed;

    if (rnKeyDown(RUIN_KEY_SPACE)) {
        game.material2Ds.mi1->tint.a -= rnTimeDelta() * 0.2;
        rnCamera2DUse(game.entities.cam);
    }

    rnFrameEnd();
    
    }

    rnSelfKill();

    return 0;
}
