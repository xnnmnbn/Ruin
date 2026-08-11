#ifndef RUIN_H
#define RUIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/* --- DLL Export/Import Definitions --- */
#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef RUIN_BUILD_DLL
        #define RUIN_API __declspec(dllexport)
    #else
        #define RUIN_API __declspec(dllimport)
    #endif
#else
    #if defined(__GNUC__) && __GNUC__ >= 4
        #define RUIN_API __attribute__((visibility("default")))
    #else
        #define RUIN_API
    #endif
#endif

typedef uint8_t RnBool;

#define RN_TRUE  1
#define RN_FALSE 0

#define RUIN_ENABLE_DEBUG

#define RUIN_DEBUG(fmt, ...) printf("[ruin] " fmt "\n", ##__VA_ARGS__)

#define RUIN_INVALID_ID          0
#define RUIN_MAX_TEXTURES        1024
#define RUIN_MAX_MATERIALS       512
#define RUIN_MAX_MESHES          1024
#define RUIN_MAX_CAMERAS         16
#define RUIN_MAX_RENDER_TARGETS  16
#define RUIN_MAX_SOUNDS          512
#define RUIN_MAX_MUSICS          32
#define RUIN_MAX_SOUND_LISTENERS 1
#define RUIN_MAX_ENTITIES        1024

typedef uint32_t RnEntity;
typedef uint32_t RnTexture;
typedef uint32_t RnMaterial2D;
typedef uint32_t RnMaterial3D;
typedef uint32_t RnSound;
typedef uint32_t RnMusic;
typedef uint32_t RnMesh;
typedef uint32_t RnRenderTarget;

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
    float brightness;
    float saturation;
    float contrast;
    float invert;
    RnColor tint;
} RnPostProcess;

typedef struct {
    RnVec3   position;
    RnVec3   rotation;
    RnVec3   scale;
    RnEntity parent;
    RnBool   dead;
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
    RnColor   tint;
    RnTexture texture;
    uint32_t  _pad[3];
} RnMaterial2DInfo;

typedef struct {
    RnTexture  albedo;
    RnTexture  normal;
    RnTexture  roughness;
    RnTexture  metallic;
    RnColor    tint;
    float      alpha;
    float      roughness_val;
    float      metallic_val;
    float      tiling_x;
    float      tiling_y;
} RnMaterial3DInfo;

typedef struct {
    RnRenderTarget rendertarget;
    float width;
    float height;
    float far;
    float near;
} RnCamera2D;

typedef struct {
    RnRenderTarget rendertarget;
    float width;
    float height;
    float far;
    float near;
    float fov;
} RnCamera3D;

typedef struct {
    RnMaterial2D material;
    int32_t      layer;
    RnBool       dead;
} RnSpriteRenderer;

typedef struct {
    RnMesh mesh;
} RnMeshRenderer;


RUIN_API RnVec2 rnVec2(float x, float y);
RUIN_API RnVec3 rnVec3(float x, float y, float z);
RUIN_API RnVec4 rnVec4(float x, float y, float z, float w);

RUIN_API RnVec2 rnVec2Sum(const RnVec2 *a, const RnVec2 *b);
RUIN_API RnVec2 rnVec2Sub(const RnVec2 *a, const RnVec2 *b);
RUIN_API RnVec2 rnVec2Mul(const RnVec2 *a, float f);
RUIN_API RnVec2 rnVec2Div(const RnVec2 *a, float f);

RUIN_API float  rnVec2Mag(const RnVec2 *v);
RUIN_API RnVec2 rnVec2Nor(const RnVec2 *v);

RUIN_API RnVec3 rnVec3Sum(const RnVec3 *a, const RnVec3 *b);
RUIN_API RnVec3 rnVec3Sub(const RnVec3 *a, const RnVec3 *b);
RUIN_API RnVec3 rnVec3Mul(const RnVec3 *a, float f);
RUIN_API RnVec3 rnVec3Div(const RnVec3 *a, float f);

RUIN_API float  rnVec3Mag(const RnVec3 *v);
RUIN_API RnVec3 rnVec3Nor(const RnVec3 *v);

RUIN_API RnVec4 rnVec4Sum(const RnVec4 *a, const RnVec4 *b);
RUIN_API RnVec4 rnVec4Sub(const RnVec4 *a, const RnVec4 *b);
RUIN_API RnVec4 rnVec4Mul(const RnVec4 *a, float f);
RUIN_API RnVec4 rnVec4Div(const RnVec4 *a, float f);

RUIN_API float  rnVec4Mag(const RnVec4 *v);
RUIN_API RnVec4 rnVec4Nor(const RnVec4 *v);


RUIN_API RnBool rnSelfInit(RnConfig *c);
RUIN_API RnBool rnSelfRunning(void);
RUIN_API void   rnSelfKill(void);

RUIN_API RnConfig *rnConfigGet(void);
RUIN_API void      rnConfigUpdatePlatform(void);
RUIN_API void      rnConfigUpdateRenderer(void);
RUIN_API void      rnConfigUpdateAudio(void);
RUIN_API void      rnConfigUpdateAll(void);
RUIN_API void      rnConfigReset(void);

RUIN_API RnPostProcess  rnDefaultPostProcess(void);
RUIN_API RnPostProcess *rnPostProcessGet(void);

RUIN_API void rnFrameBegin(void);
RUIN_API void rnFrameEnd(void);


RUIN_API RnEntity rnEntityCreate(void);
RUIN_API void     rnEntityKill(RnEntity e);
RUIN_API RnBool   rnEntityValid(RnEntity e);


RUIN_API uint8_t rnTimeFPS(void);
RUIN_API float   rnTimeDelta(void);
RUIN_API float   rnTimeElapsed(void);
RUIN_API float   rnTimeDeltaFixed(void);
RUIN_API float   rnTimeElapsedFixed(void);
RUIN_API void    rnTimeSetSpeed(float s);
RUIN_API void    rnTimeSetTargetFPS(uint8_t t);


RUIN_API RnBool rnKeyDown(RnKey k);
RUIN_API RnBool rnKeyHold(RnKey k);
RUIN_API RnBool rnKeyUp(RnKey k);

RUIN_API RnBool rnMouseDown(RnMouseButton b);
RUIN_API RnBool rnMouseHold(RnMouseButton b);
RUIN_API RnBool rnMouseUp(RnMouseButton b);
RUIN_API float  rnMouseScroll(void);
RUIN_API RnVec2 rnMouseMove(void);
RUIN_API void   rnMouseHideCursor(RnBool b);


RUIN_API RnRenderTarget rnRenderTargetCreate(uint32_t w, uint32_t h);
RUIN_API void           rnRenderTargetKill(RnRenderTarget t);

RUIN_API RnTexture rnTextureCreate(const char *path);
RUIN_API RnTexture rnTextureFromRenderTarget(RnRenderTarget t);
RUIN_API void      rnTextureKill(RnTexture t);
RUIN_API void      rnTextureCreateGPUResources(void);
RUIN_API void      rnTextureLoadToGPU(RnTexture *textures, uint32_t count);
RUIN_API void      rnTextureLoadAllToGPU(void);

RUIN_API RnMesh rnMeshLoad(const char *path);
RUIN_API void   rnMeshKill(RnMesh m);

RUIN_API RnSound rnSoundLOad(const char *path);
RUIN_API void    rnSoundKill(RnSound s);

RUIN_API RnMusic rnMusicLoad(const char *path);
RUIN_API void    rnMusicKill(RnMusic m);


RUIN_API RnMaterial2DInfo  rnDefaultMaterial2DInfo(void);
RUIN_API RnMaterial2D      rnMaterial2DCreate(RnMaterial2DInfo i);
RUIN_API RnMaterial2DInfo *rnMaterial2DGet(RnMaterial2D m);
RUIN_API void              rnMaterial2DKill(RnMaterial2D m);

RUIN_API RnMaterial3DInfo  rnDefaultMaterial3DInfo(void);
RUIN_API RnMaterial3D      rnMaterial3DCreate(RnMaterial3DInfo i);
RUIN_API RnMaterial3DInfo *rnMaterial3DGet(RnMaterial3D m);
RUIN_API void              rnMaterial3DKill(RnMaterial3D m);


RUIN_API RnTransform  rnDefaultTransform(void);
RUIN_API RnTransform *rnTransformGet(RnEntity e);
RUIN_API void         rnTransformAdd(RnEntity e, RnTransform t);
RUIN_API void         rnTransformKill(RnEntity e);
RUIN_API void         rnTransformSetDirty(RnEntity e, RnBool d);
RUIN_API void         rnTransformSortByParent(void);
RUIN_API RnVec3       rnTransformGetWorldPosition(RnEntity e);
RUIN_API RnVec3       rnTransformGetWorldRotation(RnEntity e);
RUIN_API RnVec3       rnTransformGetWorldScale(RnEntity e);

RUIN_API RnRigidBody2D *rnRigidBody2dGet(RnEntity e);
RUIN_API void           rnRigidBody2dAdd(RnEntity e, RnRigidBody2D b);
RUIN_API void           rnRigidBody2dKill(RnEntity e);

RUIN_API RnRigidBody3D *rnRigidBody3dGet(RnEntity e);
RUIN_API void           rnRigidBody3dAdd(RnEntity e, RnRigidBody3D b);
RUIN_API void           rnRigidBody3dKill(RnEntity e);

RUIN_API RnSoundListener *rnSoundListenerGet(RnEntity e);
RUIN_API void             rnSoundListenerAdd(RnEntity e, RnSoundListener l);
RUIN_API void             rnSoundListenerKill(RnEntity e);

RUIN_API RnSoundPlayer *rnSoundPlayerGet(RnEntity e);
RUIN_API void           rnSoundPlayerAdd(RnEntity e, RnSoundPlayer p);
RUIN_API void           rnSoundPlayerKill(RnEntity e);
RUIN_API void           rnSoundPlayerPlay(RnEntity e);
RUIN_API void           rnSoundPlayerStop(RnEntity e);

RUIN_API RnMusicPlayer *rnMusicPlayerGet(RnEntity e);
RUIN_API void           rnMusicPlayerAdd(RnEntity e, RnMusicPlayer p);
RUIN_API void           rnMusicPlayerKill(RnEntity e);
RUIN_API void           rnMusicPlayerPlay(RnEntity e);
RUIN_API void           rnMusicPlayerStop(RnEntity e);

RUIN_API RnCamera2D *rnCamera2DGet(RnEntity e);
RUIN_API void        rnCamera2DAdd(RnEntity e, RnCamera2D c);
RUIN_API void        rnCamera2DKill(RnEntity e);
RUIN_API void        rnCamera2DUse(RnEntity e);

RUIN_API RnCamera3D *rnCamera3DGet(RnEntity e);
RUIN_API void        rnCamera3DAdd(RnEntity e, RnCamera3D c);
RUIN_API void        rnCamera3DKill(RnEntity e);
RUIN_API void        rnCamera3DUse(RnEntity e);

RUIN_API RnSpriteRenderer  rnDefaultSpriteRenderer(void);
RUIN_API RnSpriteRenderer *rnSpriteRendererGet(RnEntity e);
RUIN_API void              rnSpriteRendererAdd(RnEntity e, RnSpriteRenderer r);
RUIN_API void              rnSpriteRendererKill(RnEntity e);
RUIN_API void              rnSpriteRendererSortByLayer(void);

RUIN_API RnMeshRenderer  rnDefaultMeshRenderer(void);
RUIN_API RnMeshRenderer *rnMeshRendererGet(RnEntity e);
RUIN_API void            rnMeshRendererAdd(RnEntity e, RnMeshRenderer r);
RUIN_API void            rnMeshRendererKill(RnEntity e);

#ifdef __cplusplus
}
#endif

#endif /* RUIN_H */
