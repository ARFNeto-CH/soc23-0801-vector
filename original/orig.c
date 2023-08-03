// https:// solarianprogrammer.com/2017/01/08/c99-c11-dynamic-array-mimics-cpp-vector-api-improvements/

#include <stdio.h>
#include "array.h"

typedef struct
{
    int x;
    int y;
} Vector2i;

int main()
{
    ARRAY_CREATE(int, arr);
    ARRAY_CREATE(Vector2i, arr2);
    // Check resize and push data
        for (int i = 0; i < 50; ++i)
    {
        ARRAY_PUSH(arr, i * i - i);
        ARRAY_PUSH(arr2, ((Vector2i){i * i, -i}));
            }
    for (size_t i = 0; i < ARRAY_SIZE(arr); ++i)
    {
        printf("%d ", arr[i]);
        
    }
    printf("\n\n");
    for (size_t i = 0; i < ARRAY_SIZE(arr2); ++i)
    {
        printf("(%d, %d)", arr2[i].x, arr2[i].y);
        
    }
    printf("\n\n");
    // Free array
    ARRAY_DESTROY(arr);
    ARRAY_DESTROY(arr2);
    return 0;
}