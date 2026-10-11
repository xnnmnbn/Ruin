#ifndef RUIN_H
#define RUIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

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

#ifdef NDEBUG
    #define RUIN_DEBUG(...) ((void*)0)
#else
    #define RUIN_DEBUG(txt, ...) fprintf(stderr, "[RUIN] " txt "\n", ##__VA_ARGS__)
#endif

#define RUIN_INVALID_ID          0
#define RUIN_MAX_TEXTURES        1024
#define RUIN_MAX_MATERIALS       512
#define RUIN_MAX_MESHES          1024
#define RUIN_MAX_CAMERAS         16
#define RUIN_MAX_RENDER_TARGETS  16
#define RUIN_MAX_SOUNDS          512
#define RUIN_MAX_MUSICS          32
#define RUIN_MAX_SOUND_LISTENERS 1
#define RUIN_MAX_ENTITIES        10000
#define RUIN_MAX_GUI_RECTS       512

typedef uint32_t RnEntity;
typedef uint32_t RnTexture;
typedef uint32_t RnSound;
typedef uint32_t RnMusic;

typedef enum {
    RN_KEY_0 = 39,
    RN_KEY_1 = 30,
    RN_KEY_2 = 31,
    RN_KEY_3 = 32,
    RN_KEY_4 = 33,
    RN_KEY_5 = 34,
    RN_KEY_6 = 35,
    RN_KEY_7 = 36,
    RN_KEY_8 = 37,
    RN_KEY_9 = 38,

    RN_KEY_A = 4,
    RN_KEY_B = 5,
    RN_KEY_C = 6,
    RN_KEY_D = 7,
    RN_KEY_E = 8,
    RN_KEY_F = 9,
    RN_KEY_G = 10,
    RN_KEY_H = 11,
    RN_KEY_I = 12,
    RN_KEY_J = 13,
    RN_KEY_K = 14,
    RN_KEY_L = 15,
    RN_KEY_M = 16,
    RN_KEY_N = 17,
    RN_KEY_O = 18,
    RN_KEY_P = 19,
    RN_KEY_Q = 20,
    RN_KEY_R = 21,
    RN_KEY_S = 22,
    RN_KEY_T = 23,
    RN_KEY_U = 24,
    RN_KEY_V = 25,
    RN_KEY_W = 26,
    RN_KEY_X = 27,
    RN_KEY_Y = 28,
    RN_KEY_Z = 29,

    RN_KEY_SPACE     = 44,
    RN_KEY_ENTER     = 40,
    RN_KEY_TAB       = 43,
    RN_KEY_ESCAPE    = 41,
    RN_KEY_BACKSPACE = 42,

    RN_KEY_F1  = 58,
    RN_KEY_F2  = 59,
    RN_KEY_F3  = 60,
    RN_KEY_F4  = 61,
    RN_KEY_F5  = 62,
    RN_KEY_F6  = 63,
    RN_KEY_F7  = 64,
    RN_KEY_F8  = 65,
    RN_KEY_F9  = 66,
    RN_KEY_F10 = 67,
    RN_KEY_F11 = 68,
    RN_KEY_F12 = 69,

    RN_KEY_LEFT_SHIFT  = 225,
    RN_KEY_RIGHT_SHIFT = 229,
    RN_KEY_LEFT_CTRL   = 224,
    RN_KEY_RIGHT_CTRL  = 228,
    RN_KEY_LEFT_ALT    = 226,
    RN_KEY_RIGHT_ALT   = 230,

    RN_KEY_UP    = 82,
    RN_KEY_DOWN  = 81,
    RN_KEY_LEFT  = 80,
    RN_KEY_RIGHT = 79
} RnKey;

typedef enum {
    RN_MOUSE_LEFT   = 1,
    RN_MOUSE_MIDDLE = 2,
    RN_MOUSE_RIGHT  = 3,
    RN_MOUSE_X1     = 4,
    RN_MOUSE_X2     = 5
} RnMouseButton;

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
    float x, y, w, h;
} RnRect;

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
    float   brightness;
    float   saturation;
    float   contrast;
    float   invert;
    RnColor tint;
} RnPostProcess;

typedef struct {
    RnVec3   position;
    RnVec3   rotation;
    RnVec3   scale;
    RnEntity parent;
    bool     dead;
} RnTransform;

typedef struct {
    RnRect    uv;
    RnColor   tint;

    RnVec2    aberration;
    uint32_t  layer;
    RnTexture texture;
    
    float     brightness;
    float     saturation;
    float     contrast;
    float     aberration_strength;
} RnRenderer2D;


typedef struct {
    RnRect    uv;
    
    RnColor   tint;

    RnVec2    anchor;
    RnVec2    pivot;
    
    RnVec2    size;
    float     rotation;
    RnTexture texture;

    uint32_t  layer;
} RnGuiRect;



typedef struct {
    float width;
    float height;
    float far;
    float near;
} RnCamera2D;

RUIN_API RnVec2 rnVec2(float x, float y);
RUIN_API RnVec3 rnVec3(float x, float y, float z);
RUIN_API RnVec4 rnVec4(float x, float y, float z, float w);

RUIN_API RnVec2 rnVec2Sum(const RnVec2 a, const RnVec2 b);
RUIN_API RnVec2 rnVec2Sub(const RnVec2 a, const RnVec2 b);
RUIN_API RnVec2 rnVec2Mul(const RnVec2 a, float f);
RUIN_API RnVec2 rnVec2Div(const RnVec2 a, float f);

RUIN_API float  rnVec2Mag(const RnVec2 v);
RUIN_API RnVec2 rnVec2Nor(const RnVec2 v);

RUIN_API RnVec3 rnVec3Sum(const RnVec3 a, const RnVec3 b);
RUIN_API RnVec3 rnVec3Sub(const RnVec3 a, const RnVec3 b);
RUIN_API RnVec3 rnVec3Mul(const RnVec3 a, float f);
RUIN_API RnVec3 rnVec3Div(const RnVec3 a, float f);

RUIN_API float  rnVec3Mag(const RnVec3 v);
RUIN_API RnVec3 rnVec3Nor(const RnVec3 v);


RUIN_API float  rnVec4Mag(const RnVec4 v);
RUIN_API RnVec4 rnVec4Nor(const RnVec4 v);





RUIN_API RnBool rnSelfInit(RnConfig *c);
RUIN_API RnBool rnSelfRunning(void);
RUIN_API void   rnSelfKill(void);

RUIN_API RnConfig  rnDefaultConfig(void);
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


RUIN_API uint32_t rnTimeFPS(void);
RUIN_API float    rnTimeDelta(void);
RUIN_API float    rnTimeElapsed(void);
RUIN_API float    rnTimeDeltaFixed(void);
RUIN_API float    rnTimeElapsedFixed(void);
RUIN_API void     rnTimeSetSpeed(float s);
RUIN_API void     rnTimeSetTargetFPS(uint8_t t);


RUIN_API RnBool rnKeyDown(RnKey k);
RUIN_API RnBool rnKeyHold(RnKey k);
RUIN_API RnBool rnKeyUp(RnKey k);

RUIN_API RnBool rnMouseDown(RnMouseButton b);
RUIN_API RnBool rnMouseHold(RnMouseButton b);
RUIN_API RnBool rnMouseUp(RnMouseButton b);
RUIN_API float  rnMouseScroll(void);
RUIN_API RnVec2 rnMouseMove(void);
RUIN_API void   rnMouseHideCursor(RnBool b);

RUIN_API RnTexture rnTextureCreate(const char *path, uint32_t w, uint32_t h);
RUIN_API void      rnTextureKill(RnTexture t);
RUIN_API void      rnTextureCreateGpuResources(void);
RUIN_API void      rnTextureLoadToGpuOffscreen(RnTexture *textures, uint32_t count);

RUIN_API RnSound rnSoundLOad(const char *path);
RUIN_API void    rnSoundKill(RnSound s);
RUIN_API void    rnSoundPlay(RnSound s);
RUIN_API void    rnSoundPause(RnSound s, RnBool pause);

RUIN_API RnMusic rnMusicLoad(const char *path);
RUIN_API void    rnMusicKill(RnMusic m);
RUIN_API void    rnMusicPlay(RnMusic m);
RUIN_API void    rnMusicPause(RnMusic m, RnBool pause);





RUIN_API RnTransform  rnDefaultTransform(void);
RUIN_API RnTransform *rnTransformGet(RnEntity e);
RUIN_API void         rnTransformAdd(RnEntity e, RnTransform t);
RUIN_API void         rnTransformKill(RnEntity e);
RUIN_API void         rnTransformSetDirty(RnEntity e, RnBool d);
RUIN_API void         rnTransformSortByParent(void);
RUIN_API RnVec3       rnTransformGetWorldPosition(RnEntity e);
RUIN_API RnVec3       rnTransformGetWorldRotation(RnEntity e);
RUIN_API RnVec3       rnTransformGetWorldScale(RnEntity e);


RUIN_API RnRenderer2D  rnDefaultRenderer2D(void);
RUIN_API RnRenderer2D *rnRenderer2DGet(RnEntity e);
RUIN_API void          rnRenderer2DAdd(RnEntity e, RnRenderer2D r);
RUIN_API void          rnRenderer2DKill(RnEntity e);
RUIN_API void          rnRenderer2DSortByLayer(void);

RUIN_API RnCamera2D *rnCamera2DGet(RnEntity e);
RUIN_API void        rnCamera2DAdd(RnEntity e, RnCamera2D c);
RUIN_API void        rnCamera2DKill(RnEntity e);
RUIN_API void        rnCamera2DUse(RnEntity e);

#ifdef __cplusplus
}
#endif

#endif /* RUIN_H */
