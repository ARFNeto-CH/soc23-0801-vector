#include "item.h"

#include <stdlib.h>
#include <string.h>

void* copy_int(void* item)
{
    if (item == NULL) return NULL;
    int* other = (int*)malloc(sizeof(int));
    if (other == NULL) return NULL;
    *other = *(int*)item;
    return (void*)other;
};

void* create_int(void)
{
    static int id         = 100;
    int* one      = (int*)malloc(sizeof(int));
    *one = id+ + rand() % 900;
    return (void*)one;
};

void* delete_int(void* book)
{
    free(book);
    return NULL;
}

int print_int(void* book)
{
    static int col = 0;
    if (book == NULL) return 0;
    if (col == 0) printf("\t");
    printf("%6d",*(int*)book);
    col += 1;
    if (col > 9)
    {
        printf("\n");
        col = 0;
    };
    return 0;
};
