#ifndef CVEC_H
#define CVEC_H

#include "stdlib.h"
#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cvec {
    size_t len;
    size_t cap;
    void  *data;
} cvec;


#define cvec_t(__v, __t) ((__t*)(__v.data))

#define cvec_init(_v) do { \
_v.len  = 0;               \
_v.cap  = 0;               \
_v.data = NULL;            \
} while (0)


#define cvec_kill(_v) do { \
free(_v.data);             \
_v.len  = 0;               \
_v.cap  = 0;               \
_v.data = NULL;            \
} while (0)

#define cvec_push(_v, _d, _t) do {                      \
if (_v.len >= _v.cap) {                                 \
    _v.cap += 256;                                      \
    void* __new_data = malloc(sizeof(_t) * _v.cap);     \
    for (size_t __i = 0; __i < _v.len; __i++) {         \
        ((_t*)__new_data)[__i] = ((_t*)(_v.data))[__i]; \
    }                                                   \
    free(_v.data);                                      \
    _v.data = __new_data;                               \
}                                                       \
((_t*)(_v.data))[_v.len] = _d;                          \
_v.len += 1;                                            \
} while (0)

#define cvec_remove(_v, _d, _t) do {                           \
for (size_t __i = 0; __i < _v.len; __i++) {                    \
    if (((_t*)_v.data)[__i] == _d) {                           \
        if (__i == (_v.len - 1)) { _v.len -= 1; break; }       \
        for (size_t __j = __i; __j < _v.len - 1; __j++) {      \
            ((_t*)(_v.data))[__j] = ((_t*)(_v.data))[__j + 1]; \
        }                                                      \
        _v.len -= 1; break;                                    \
    }                                                          \
}                                                              \
} while (0)

#define cvec_remove_at(_v, _i, _t) do {                    \
for (size_t __i = _i; __i < (_v).len - 1; __i++) {         \
    ((_t*)((_v).data))[__i] = ((_t*)((_v).data))[__i + 1]; \
}                                                          \
(_v).len -= 1;                                             \
} while (0)

#define cvec_pop(_v, _t, _p) do {      \
    _p = ((_t*)(_v.data))[_v.len - 1]; \
    _v.len -= 1;                       \
} while (0)

#define cvec_index(_v, _d, _t, _i)  do {         \
for (size_t __i = 0; __i < _v.len; __i++) {      \
    if (((_t*)(_v.data))[__i] == _d) {           \
        _i = __i; break;                         \
    }                                            \
}                                                \
} while (0)

#ifdef __cplusplus
}
#endif

#endif
