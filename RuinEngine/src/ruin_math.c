#include "ruin.h"
#include "math.h"


RnVec2 rnVec2(float x, float y) {
    return (RnVec2){
        .x = x,
        .y = y
    };
}
RnVec3 rnVec3(float x, float y, float z) {
    return (RnVec3){
        .x = x,
        .y = y,
        .z = z
    };
}
RnVec4 rnVec4(float x, float y, float z, float w) {
    return (RnVec4){
        .x = x,
        .y = y,
        .z = z,
        .w = w
    };
}


RnVec2 rnVec2Sum(const RnVec2 *a, const RnVec2 *b) {
    return (RnVec2){
        .x = a->x + b->x,
        .y = a->y + b->y
    };
}
RnVec2 rnVec2Sub(const RnVec2 *a, const RnVec2 *b) {
    return (RnVec2){
        .x = a->x - b->x,
        .y = a->y - b->y
    };
}
RnVec2 rnVec2Mul(const RnVec2 *a, float f) {
    return (RnVec2){
        .x = a->x * f,
        .y = a->y * f
    };
}
RnVec2 rnVec2Div(const RnVec2 *a, float f) {
    return (RnVec2){
        .x = a->x / f,
        .y = a->y / f
    };
}

float rnVec2Mag(const RnVec2 *v) {
    return sqrtf(v->x * v->x + v->y * v->y);
}
RnVec2 rnVec2Nor(const RnVec2 *v) {
    const float m = rnVec2Mag(v);

    return (RnVec2){
        .x = (float)v->x / m,
        .y = (float)v->y / m
    };
}

RnVec3 rnVec3Sum(const RnVec3 *a, const RnVec3 *b) {
    return (RnVec3){
        .x = a->x + b->x,
        .y = a->y + b->y,
        .z = a->z + b->z
    };
}
RnVec3 rnVec3Sub(const RnVec3 *a, const RnVec3 *b) {
    return (RnVec3){
        .x = a->x - b->x,
        .y = a->y - b->y,
        .z = a->z - b->z
    };
}
RnVec3 rnVec3Mul(const RnVec3 *a, float f) {
    return (RnVec3){
        .x = a->x * f,
        .y = a->y * f,
        .z = a->z * f
    };
}
RnVec3 rnVec3Div(const RnVec3 *a, float f) {
    return (RnVec3){
        .x = a->x / f,
        .y = a->y / f,
        .z = a->z / f
    };
}

float rnVec3Mag(const RnVec3 *v) {
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}
RnVec3 rnVec3Nor(const RnVec3 *v) {
    const float m = rnVec3Mag(v);
    return (RnVec3){
        .x = v->x / m,
        .y = v->y / m,
        .z = v->z / m
    };
}

RnVec4 rnVec4Sum(const RnVec4 *a, const RnVec4 *b) {
    return (RnVec4){
        .x = a->x + b->x,
        .y = a->y + b->y,
        .z = a->z + b->z,
        .w = a->w + b->w
    };
}
RnVec4 rnVec4Sub(const RnVec4 *a, const RnVec4 *b) {
    return (RnVec4){
        .x = a->x - b->x,
        .y = a->y - b->y,
        .z = a->z - b->z,
        .w = a->w - b->w
    };
}
RnVec4 rnVec4Mul(const RnVec4 *a, float f) {
    return (RnVec4){
        .x = a->x * f,
        .y = a->y * f,
        .z = a->z * f,
        .w = a->w * f
    };
}
RnVec4 rnVec4Div(const RnVec4 *a, float f) {
    return (RnVec4){
        .x = a->x / f,
        .y = a->y / f,
        .z = a->z / f,
        .w = a->w / f
    };
}

float rnVec4Mag(const RnVec4 *v) {
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->w);
}
RnVec4 rnVec4Nor(const RnVec4 *v) {
    const float m = rnVec4Mag(v);
    return (RnVec4){
        .x = v->x / m,
        .y = v->y / m,
        .z = v->z / m,
        .w = v->w / m
    };
}
