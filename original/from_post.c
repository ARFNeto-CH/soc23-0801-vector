#include <stdio.h>

// arr.h
typedef enum
{
    LENGTH,
    STRIDE,
    CAPACITY,
    NUM_PROPERTIES
} Header;

#define arr_create(type) _arr_create(sizeof(type))
#define arr_add(arr, num) _arr_add(arr, &num)
#define arr_delete(arr)                                    \
    free(((size_t*)arr) - NUM_PROPERTIES)

// arr.c
void* _arr_create(size_t stride)
{
    size_t* arr =
        malloc(sizeof(size_t) * NUM_PROPERTIES + stride);
    if (arr == NULL)
    {
        fprintf(
            stderr,
            "Error allocating memory in _arr_create()\n");
        exit(1);
    }

    arr[LENGTH]   = 0;      /* size */
    arr[STRIDE]   = stride; /* stride */
    arr[CAPACITY] = 1;

    return (void*)(arr + NUM_PROPERTIES);
}

void _arr_add(void* arr, void* num)
{
    size_t* raw = ((size_t*)arr) - NUM_PROPERTIES;
    printf("LENGTH: %d\n", raw[LENGTH]);
    if (raw[LENGTH] == raw[CAPACITY])
    {
        raw[CAPACITY] = raw[CAPACITY] * 2;
        raw           = realloc(
            raw, sizeof(size_t) * NUM_PROPERTIES +
                     raw[CAPACITY] * raw[STRIDE]);
        if (raw == NULL)
        {
            fprintf(
                stderr,
                "Error reallocating memory in "
                "_arr_add()\n");
            exit(1);
        }
    }
    memcpy(
        (char*)arr + raw[LENGTH] * raw[STRIDE], num, raw[STRIDE]);
    raw[LENGTH] = raw[LENGTH] + 1;
}
