// array.h - Helper macros to create and manipulate a
// dynamic C array
#pragma once
#include <stdlib.h>

#define ARRAY_CREATE(T, arr)                               \
    T* arr = NULL;                                         \
    do {                                                   \
        size_t* raw = malloc(2 * sizeof(size_t));          \
        raw[0]      = 0;                                   \
        raw[1]      = 0;                                   \
        arr         = (void*)&raw[2];                      \
    } while (0)

#define ARRAY_DESTROY(arr)                                 \
    do {                                                   \
        size_t* raw = ((size_t*)(arr)-2);                  \
        free(raw);                                         \
        arr = NULL;                                        \
    } while (0)

#define ARRAY_SIZE(ARR) (*((size_t*)ARR - 2))

#define ARRAY_CAPACITY(ARR) (*((size_t*)ARR - 1))

#define ARRAY_PUSH(arr, value)                             \
    do {                                                   \
        size_t* raw = ((size_t*)(arr)-2);                  \
        raw[0]      = raw[0] + 1;                          \
        if (raw[1] == 0)                                   \
        {                                                  \
            raw[1] = 1;                                    \
            raw    = realloc(                              \
                raw, 2 * sizeof(size_t) +               \
                         raw[1] * sizeof((value)));     \
            (arr) = (void*)&raw[2];                        \
        }                                                  \
        if (raw[0] > raw[1])                               \
        {                                                  \
            raw[1] = 2 * raw[1];                           \
            raw    = realloc(                              \
                raw, 2 * sizeof(size_t) +               \
                         raw[1] * sizeof((value)));     \
            (arr) = (void*)&raw[2];                        \
        }                                                  \
        arr[raw[0] - 1] = (value);                         \
    } while (0)
