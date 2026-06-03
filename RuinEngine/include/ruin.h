#ifndef RUIN_H
#define RUIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

typedef uint8_t RnBool;

#define RN_TRUE  1
#define RN_FALSE 0

// #define RN_ENABLE_DEBUG

#define RUIN_DEBUG(fmt, ...) printf("[ruin] " fmt "\n", ##__VA_ARGS__)

#define RUIN_INVALID_ID          0
#define RUIN_MAX_TEXTURES        1024
#define RUIN_MAX_MATERIALS       512
#define RUIN_MAX_MESHES          1024
#define RUIN_MAX_CAMERAS         16
#define RUIN_MAX_RENDER_TARGETS  32
#define RUIN_MAX_SOUNDS          512
#define RUIN_MAX_MUSICS          32
#define RUIN_MAX_SOUND_LISTENERS 1
#define RUIN_MAX_ENTITIES        20000

typedef uint32_t RnEntity;
typedef uint16_t RnTexture;
typedef uint16_t RnMaterial;
typedef uint16_t RnSound;
typedef uint16_t RnMusic;
typedef uint16_t RnMesh;
typedef uint8_t  RnRenderTarget;

typedef enum {
    RUIN_KEY_0 = 39,
    RUIN_KEY_1 = 30,
    RUIN_KEY_2 = 31,
    RUIN_KEY_3 = 32,
    RUIN_KEY_4 = 33,
    RUIN_KEY_5 = 34,
    RUIN_KEY_6 = 35,
    RUIN_KEY_7 = 36,
    RUIN_KEY_8 = 37,
    RUIN_KEY_9 = 38,

    RUIN_KEY_A = 4,
    RUIN_KEY_B = 5,
    RUIN_KEY_C = 6,
    RUIN_KEY_D = 7,
    RUIN_KEY_E = 8,
    RUIN_KEY_F = 9,
    RUIN_KEY_G = 10,
    RUIN_KEY_H = 11,
    RUIN_KEY_I = 12,
    RUIN_KEY_J = 13,
    RUIN_KEY_K = 14,
    RUIN_KEY_L = 15,
    RUIN_KEY_M = 16,
    RUIN_KEY_N = 17,
    RUIN_KEY_O = 18,
    RUIN_KEY_P = 19,
    RUIN_KEY_Q = 20,
    RUIN_KEY_R = 21,
    RUIN_KEY_S = 22,
    RUIN_KEY_T = 23,
    RUIN_KEY_U = 24,
    RUIN_KEY_V = 25,
    RUIN_KEY_W = 26,
    RUIN_KEY_X = 27,
    RUIN_KEY_Y = 28,
    RUIN_KEY_Z = 29,

    RUIN_KEY_SPACE     = 44,
    RUIN_KEY_ENTER     = 40,
    RUIN_KEY_TAB       = 43,
    RUIN_KEY_ESCAPE    = 41,
    RUIN_KEY_BACKSPACE = 42,

    RUIN_KEY_F1  = 58,
    RUIN_KEY_F2  = 59,
    RUIN_KEY_F3  = 60,
    RUIN_KEY_F4  = 61,
    RUIN_KEY_F5  = 62,
    RUIN_KEY_F6  = 63,
    RUIN_KEY_F7  = 64,
    RUIN_KEY_F8  = 65,
    RUIN_KEY_F9  = 66,
    RUIN_KEY_F10 = 67,
    RUIN_KEY_F11 = 68,
    RUIN_KEY_F12 = 69,

    RUIN_KEY_LEFT_SHIFT  = 225,
    RUIN_KEY_RIGHT_SHIFT = 229,
    RUIN_KEY_LEFT_CTRL   = 224,
    RUIN_KEY_RIGHT_CTRL  = 228,
    RUIN_KEY_LEFT_ALT    = 226,
    RUIN_KEY_RIGHT_ALT   = 230,

    RUIN_KEY_UP    = 82,
    RUIN_KEY_DOWN  = 81,
    RUIN_KEY_LEFT  = 80,
    RUIN_KEY_RIGHT = 79
} RnKey;

typedef enum {
    RUIN_MOUSE_LEFT   = 1,
    RUIN_MOUSE_MIDDLE = 2,
    RUIN_MOUSE_RIGHT  = 3,
    RUIN_MOUSE_X1     = 4,
    RUIN_MOUSE_X2     = 5
} RnMouseButton;

typedef enum {
    RUIN_MAT3D_MASK_ALBEDO        = 1 << 0,
    RUIN_MAT3D_MASK_NORMAL        = 1 << 1,
    RUIN_MAT3D_MASK_ROUGHNESS     = 1 << 2,
    RUIN_MAT3D_MASK_METALLIC      = 1 << 3,
    RUIN_MAT3D_MASK_TINT          = 1 << 4,
    RUIN_MAT3D_MASK_ALPHA         = 1 << 5,
    RUIN_MAT3D_MASK_ROUGHNESS_VAL = 1 << 6,
    RUIN_MAT3D_MASK_METALLIC_VAL  = 1 << 7,
    RUIN_MAT3D_MASK_TILING_X      = 1 << 8,
    RUIN_MAT3D_MASK_TILING_Y      = 1 << 9,
    RUIN_MAT3D_MASK_ALL           = RUIN_MAT3D_MASK_ALBEDO    | RUIN_MAT3D_MASK_NORMAL        |
                                    RUIN_MAT3D_MASK_METALLIC  | RUIN_MAT3D_MASK_METALLIC_VAL  |
                                    RUIN_MAT3D_MASK_ROUGHNESS | RUIN_MAT3D_MASK_ROUGHNESS_VAL |
                                    RUIN_MAT3D_MASK_TINT      | RUIN_MAT3D_MASK_ALPHA         |
                                    RUIN_MAT3D_MASK_TILING_X  | RUIN_MAT3D_MASK_TILING_Y
} RnMaterialMask;

typedef enum {
    RUIN_PROJECTION_ORTHOGRAPHIC,
    RUIN_PROJECTION_PERSPECTIVE
} RnProjection;

typedef enum {
    RUIN_RIGIDBODY_TYPE_DYNAMIC,
    RUIN_RIGIDBODY_TYPE_STATIC,
    RUIN_RIGIDBODY_TYPE_KINEMATIC
} RnRigidBodyType;

typedef enum {
    RUIN_RIGIDBODY2D_SHAPE_SQUARE,
    RUIN_RIGIDBODY2D_SHAPE_CIRCLE
} RnRigidBody2DShape;

typedef enum {
    RUIN_RIGIDBODY3D_SHAPE_CUBE,
    RUIN_RIGIDBODY3D_SHAPE_SPHERE,
    RUIN_RIGIDBODY3D_SHAPE_CYLINDER
} RnRigidBody3DShape;





typedef struct {
    float x, y;
} RnVec2;

typedef struct {
    float x, y, z;
} RnVec3;

typedef struct {
    float x, y, z, w;
} RnVec4;

typedef struct {
    float r, g, b, a;
} RnColor;



typedef struct {
    const char *title;
    uint32_t    width;
    uint32_t    height;
    RnBool      borderless;
    RnBool      resizable;
    RnBool      fullscreen;
} RnConfigWindow;

typedef struct {
    RnBool   vsync;
    uint8_t  multisampling;
    uint8_t  max_frames_in_flight;
    float    max_anisotropy;
    uint32_t resolution_x;
    uint32_t resolution_y;
} RnConfigRenderer;

typedef struct {
    float sound_volume;
    float music_volume;
} RnConfigAudio;


typedef struct {
    RnConfigWindow   window;
    RnConfigRenderer renderer;
    RnConfigAudio    audio;
} RnConfig;


typedef struct {
    RnVec3      position;
    RnVec3      rotation;
    RnVec3      scale;
    RnEntity    parent;
    RnBool      dirty;
} RnTransform;

typedef struct {
    RnVec3 gravity;
} RnPhysicsWorld;

typedef struct {
    RnRigidBodyType    type;
    RnRigidBody2DShape shape;
    RnVec3             position;
    RnVec3             rotation;
    RnVec3             scale;
    RnVec3             velocity;
    RnVec3             angular_velocity;
    float              mass;
    float              friction;
    float              restitution;
    RnBool             ignore_gravity;
} RnRigidBody2D;

typedef struct {
    RnRigidBodyType    type;
    RnRigidBody3DShape shape;
    RnVec3             position;
    RnVec3             rotation;
    RnVec3             scale;
    RnVec3             velocity;
    RnVec3             angular_velocity;
    float              mass;
    float              friction;
    float              restitution;
    RnBool             ignore_gravity;
} RnRigidBody3D;

typedef struct {
    RnVec3 position;
    RnVec3 rotation;
    RnVec3 velocity;
} RnSoundListener;

typedef struct {
    RnSound sound;
    float   volume;
} RnSoundPlayer;

typedef struct {
    RnMusic music;
    float   volume;
} RnMusicPlayer;


typedef struct {
    RnMaterialMask masks;
    RnTexture      albedo;
    RnTexture      normal;
    RnTexture      roughness;
    RnTexture      metallic;
    RnColor        tint;
    float          alpha;
    float          roughness_val;
    float          metallic_val;
    float          tiling_x;
    float          tiling_y;
} RnMaterialInfo;

typedef struct {
    int32_t width;
    int32_t height;
    RnBool  depth;
} RnRenderTargetInfo;

typedef struct {
    RnRenderTarget target;
    float          width;
    float          height;
    float          far;
    float          near;
} RnCamera2D;

typedef struct {
    RnRenderTarget target;
    float          width;
    float          height;
    float          far;
    float          near;
    float          fov;
} RnCamera3D;

typedef struct {
    RnTexture texture;
    RnColor   tint;
    float     opacity;
} RnSpriteRenderer;

typedef struct {
    RnMesh mesh;
} RnMeshRenderer;

RnVec2 rnVec2(float x, float y);
RnVec3 rnVec3(float x, float y, float z);
RnVec4 rnVec4(float x, float y, float z, float w);

RnVec2 rnVec2Sum(const RnVec2 *a, const RnVec2 *b);
RnVec2 rnVec2Sub(const RnVec2 *a, const RnVec2 *b);
RnVec2 rnVec2Mul(const RnVec2 *a, float f);
RnVec2 rnVec2Div(const RnVec2 *a, float f);

float  rnVec2Mag(const RnVec2 *v);
RnVec2 rnVec2Nor(const RnVec2 *v);

RnVec3 rnVec3Sum(const RnVec3 *a, const RnVec3 *b);
RnVec3 rnVec3Sub(const RnVec3 *a, const RnVec3 *b);
RnVec3 rnVec3Mul(const RnVec3 *a, float f);
RnVec3 rnVec3Div(const RnVec3 *a, float f);

float  rnVec3Mag(const RnVec3 *v);
RnVec3 rnVec3Nor(const RnVec3 *v);

RnVec4 rnVec4Sum(const RnVec4 *a, const RnVec4 *b);
RnVec4 rnVec4Sub(const RnVec4 *a, const RnVec4 *b);
RnVec4 rnVec4Mul(const RnVec4 *a, float f);
RnVec4 rnVec4Div(const RnVec4 *a, float f);

float  rnVec4Mag(const RnVec4 *v);
RnVec4 rnVec4Nor(const RnVec4 *v);

RnBool rnSelfInit(RnConfig *c);
RnBool rnSelfRunning(void);
void   rnSelfKill(void);

RnConfig *rnConfigGet(void);
void      rnConfigUpdatePlatform(void);
void      rnConfigUpdateRenderer(void);
void      rnConfigUpdateAudio(void);
void      rnConfigUpdateAll(void);
void      rnConfigReset(void);

void rnFrameBegin(void);
void rnFrameEnd(void);

RnEntity rnEntityCreate(void);
void     rnEntityKill(RnEntity e);
RnBool   rnEntityValid(RnEntity e);

uint8_t rnTimeFPS(void);
float   rnTimeDelta(void);
float   rnTimeElapsed(void);
float   rnTimeDeltaFixed(void);
float   rnTimeElapsedFixed(void);
void    rnTimeSetSpeed(float s);
void    rnTimeSetTargetFPS(uint8_t t);

RnBool rnKeyDown(RnKey k);
RnBool rnKeyHold(RnKey k);
RnBool rnKeyUp(RnKey k);

RnBool rnMouseDown(RnMouseButton b);
RnBool rnMouseHold(RnMouseButton b);
RnBool rnMouseUp(RnMouseButton b);
float  rnMouseScroll(void);

RnRenderTarget rnRenderTargetCreate(RnRenderTargetInfo *i);
void           rnRenderTargetKill(RnRenderTarget t);

RnTexture rnTextureLoad(const char *path);
RnTexture rnTextureFromRenderTarget(RnRenderTarget t);
void      rnTextureKill(RnTexture t);

RnMesh rnMeshLoad(const char *path);
void   rnMeshKill(RnMesh m);

RnSound rnSoundLOad(const char *path);
void    rnSoundKill(RnSound s);

RnMusic rnMusicLoad(const char *path);
void    rnMusicKill(RnMusic m);

RnMaterial rnMaterialCreate(const RnMaterialInfo *i);
void       rnMaterialUpdate(RnMaterial m, const RnMaterialInfo *i);
void       rnMaterialKill(RnMaterial m);

RnTransform  rnDefaultTransform(void);
RnTransform *rnEntityTransformGet(RnEntity e);
void         rnEntityTransformAdd(RnEntity e, RnTransform t);
void         rnEntityTransformKill(RnEntity e);
void         rnEntityTransformSetDirty(RnEntity e, RnBool d);
RnVec3       rnEntityTransformGetWorldPosition(RnEntity e);
RnVec3       rnEntityTransformGetWorldRotation(RnEntity e);
RnVec3       rnEntityTransformGetWorldScale(RnEntity e);

RnRigidBody2D *rnEntityRigidBody2dGet(RnEntity e);
void           rnEntityRigidBody2dAdd(RnEntity e, RnRigidBody2D b);
void           rnEntityRigidBody2dKill(RnEntity e);

RnRigidBody3D *rnEntityRigidBody3dGet(RnEntity e);
void           rnEntityRigidBody3dAdd(RnEntity e, RnRigidBody3D b);
void           rnEntityRigidBody3dKill(RnEntity e);

RnSoundListener *rnEntitySoundListenerGet(RnEntity e);
void             rnEntitySoundListenerAdd(RnEntity e, RnSoundListener l);
void             rnEntitySoundListenerKill(RnEntity e);

RnSoundPlayer *rnEntitySoundPlayerGet(RnEntity e);
void           rnEntitySoundPlayerAdd(RnEntity e, RnSoundPlayer p);
void           rnEntitySoundPlayerKill(RnEntity e);
void           rnEntitySoundPlayerPlay(RnEntity e);
void           rnEntitySoundPlayerStop(RnEntity e);

RnMusicPlayer *rnEntityMusicPlayerGet(RnEntity e);
void           rnEntityMusicPlayerAdd(RnEntity e, RnMusicPlayer p);
void           rnEntityMusicPlayerKill(RnEntity e);
void           rnEntityMusicPlayerPlay(RnEntity e);
void           rnEntityMusicPlayerStop(RnEntity e);



RnCamera2D *rnEntityCamera2dGet(RnEntity e);
void        rnEntityCamera2dAdd(RnEntity e, RnCamera2D c);
void        rnEntityCamera2dKill(RnEntity e);
void        rnEntityCamera2dSetRenderTarget(RnEntity e, RnRenderTarget t);

RnCamera3D *rnEntityCamera3dGet(RnEntity e);
void        rnEntityCamera3dAdd(RnEntity e, RnCamera3D c);
void        rnEntityCamera3dKill(RnEntity e);
void        rnEntityCamera3dSetRenderTarget(RnEntity e, RnRenderTarget t);

void rnEntityCameraUse(RnEntity e, RnProjection p);

void rnEntitySpriteRendererAdd(RnEntity e, RnSpriteRenderer r);
void rnEntitySpriteRendererKill(RnEntity e);

void rnEntityMeshRendererAdd(RnEntity e, RnMeshRenderer r);
void rnEntityMeshRendererKill(RnEntity e);




#ifdef __cplusplus
}
#endif

#endif
