#ifndef __h_clists
#define __h_clists

#include <stdint.h>

typedef struct {
    uint8_t *data;
    uint32_t data_size;
    uint32_t data_count;
    uint32_t total_size;
} c_fixed_heap;

c_fixed_heap fn_fixed_heap_new(uint32_t s, uint32_t c);
void         fn_fixed_heap_kill(c_fixed_heap *h);
void        *fn_fixed_heap_at(c_fixed_heap *h, uint32_t idx);
void        *fn_fixed_heap_last(c_fixed_heap *h);

typedef struct {
    uint8_t *data;
    uint32_t data_size;
    uint32_t data_count;
    uint32_t data_cap;
} c_dyn_heap;

c_dyn_heap fn_dyn_heap_new(uint32_t s);
void       fn_dyn_heap_kill(c_dyn_heap *h);
void       fn_dyn_heap_push(c_dyn_heap *h, void *data);
void       fn_dyn_heap_remove(c_dyn_heap *h, void *d, uint8_t fast);
void       fn_dyn_heap_remove_at(c_dyn_heap *h, uint32_t idx, uint8_t fast);
void      *fn_dyn_heap_at(c_dyn_heap *h, uint32_t idx);
void      *fn_dyn_heap_pop(c_dyn_heap *h);
void      *fn_dyn_heap_last(c_dyn_heap *h);


#define c_array_decl(T, N) typedef struct { \
    T data[N]; \
} c_array_ ## T ## _ ## N

#define c_array_t(T, N) c_array_ ## T ## _ ## N


#ifdef CLISTS_IMPLEMENTATION
#include <stdlib.h>
#include <string.h>



c_fixed_heap fn_fixed_heap_new(uint32_t s, uint32_t c) {
    c_fixed_heap h = {0};
    h.data_size = s;
    h.data_count = c;
    h.total_size = s * c;
    h.data = (uint8_t *)malloc(h.total_size);

    return h;
}

void fn_fixed_heap_kill(c_fixed_heap *h) {
    free(h->data);
    *h = (c_fixed_heap){0};
}

void *fn_fixed_heap_at(c_fixed_heap *h, uint32_t idx) {
    return &h->data[idx * h->data_size];
}

void *fn_fixed_heap_last(c_fixed_heap *h) {
    return &h->data[(h->data_count - 1) * h->data_size];
}



c_dyn_heap fn_dyn_heap_new(uint32_t s) {
    c_dyn_heap h = {0};
    h.data_size = s;

    return h;
}

void fn_dyn_heap_kill(c_dyn_heap *h) {
    free(h->data);
    *h = (c_dyn_heap){0};
}

void fn_dyn_heap_push(c_dyn_heap *h, void *data) {
    if (h->data_count == h->data_cap) {
        uint32_t new_cap = (h->data_cap == 0) ? 8 : (h->data_cap * 2);
        h->data = (uint8_t *)realloc(h->data, new_cap * h->data_size);
        h->data_cap = new_cap;
    }

    uint32_t offset = h->data_count * h->data_size;
    memcpy(&h->data[offset], data, h->data_size);

    h->data_count += 1;
}

void *fn_dyn_heap_at(c_dyn_heap *h, uint32_t idx) {
    return &h->data[h->data_size * idx];
}

void fn_dyn_heap_remove_at(c_dyn_heap *h, uint32_t idx, uint8_t fast) {
    uint32_t at = h->data_size * idx;

    if (fast) {
        uint8_t *last = &h->data[h->data_size * (h->data_count - 1)];
        memcpy(&h->data[at], last, h->data_size);
        h->data_count -= 1;
        return;
    }

    uint32_t total_bytes = h->data_size * h->data_count;
    uint32_t bytes_to_move = total_bytes - (at + h->data_size);
    
    if (bytes_to_move > 0) {
        memmove(&h->data[at], &h->data[at + h->data_size], bytes_to_move);
    }

    h->data_count -= 1;
}

void fn_dyn_heap_remove(c_dyn_heap *h, void *d, uint8_t fast) {
    for (uint32_t i = 0; i < h->data_count; i++) {
        uint8_t *current = &h->data[i * h->data_size];
        if (memcmp(current, d, h->data_size) == 0) {
            fn_dyn_heap_remove_at(h, i, fast);
            return;
        }
    }
}

void *fn_dyn_heap_pop(c_dyn_heap *h) {
    h->data_count -= 1;
    return &h->data[h->data_size * h->data_count];
}

void *fn_dyn_heap_last(c_dyn_heap *h) {
    return &h->data[h->data_size * (h->data_count - 1)];
}

#endif

#endif
