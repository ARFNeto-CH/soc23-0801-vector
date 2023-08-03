#pragma once
#include "container.h"

int ctn_capacity(Container* ctn)
{
    if (ctn == NULL) return -1;
    return (int) ctn->limit;
}

int ctn_clear(Container* ctn)
{
    if (ctn == NULL) return -1;
    for (size_t ix = 0; ix < ctn->size; ix += 1)
        ctn->destroy((void*)(ctn->item[ix])); // free
    ctn->size = 0; // mark empty
    return 0;
}

Container* ctn_create(
    size_t size, void* (*copy)(void*),
    void* (*destroy)(void*), int (*show)(void*))
{
    Container* ctn =
        (Container*)malloc(sizeof(Container) * size);
    if (ctn == NULL) return NULL;
    ctn->limit = size;  // limit
    ctn->size  = 0;     // now empty, of course
    ctn->item = (void**)malloc(size * sizeof(void*));
    if (ctn->item == NULL)
    {  // could not allocate
        free(ctn);
        return NULL;
    }
    ctn->copy    = copy;
    ctn->destroy = destroy;
    ctn->show    = show;
    return ctn;
}

Container* ctn_destroy(Container* ctn)
{  // to destroy a container we need to know how to
    // delete the voids: they can allocate memory
    if (ctn == NULL) return NULL;
    for (size_t ix = 0; ix < ctn->size; ix += 1)
        ctn->destroy(ctn->item[ix]);
    return NULL;
}

int ctn_empty(Container* ctn)
{ 
    return ctn->size == 0; }

int ctn_pop_back(Container* ctn)
{ 
    if (ctn == NULL) return -1;
    if (ctn->size == 0) return -2;
    ctn->destroy((void*)ctn->item[(ctn->size)-1]);
    ctn->size -= 1;
    printf("    pop_back(): size is now %d\n", ctn_size(ctn));
    return 0;
}

int ctn_push_back(void* item, Container* ctn)
{  // it is not wise to insert just a pointer
    // that can be free'd elsewhere:
    // it must be a copy. But an void can allocate
    // memory so we need to rely on user supplied
    // method
    if (item == NULL) return -1;       // no void
    if (ctn == NULL) return -2;        // no container
    if (ctn->size == ctn->limit)
        return -3;                     // container full
    if (ctn->copy == NULL) return -4;  // no copy function
    ctn->item[ctn->size] = ctn->copy(item);
    if (ctn->item[ctn->size] == NULL)
        return -5;  // error on copy
    ctn->size += 1;
    return 0;
}

int ctn_resize(Container* ctn, const size_t size) 
{ 
    // there is nothing special in resize
    // just check the pointers and call
    // realloc. If the size is getting 
    // smaller must call destroy to free
    // memory for items
    return 0;
}

int ctn_show(Container* col)
{
    if (col == NULL) return -1;
    printf(
        "    there are %llu of %llu items\n\n", col->size,
        col->limit);
    if (col->show == NULL) return -1;  // no show function
    for (size_t i = 0; i < col->size; i += 1)
        col->show(col->item[i]);
    printf("[end of listing]\n");
    return 0;
}

int ctn_size(Container* ctn) { return (int) ctn->size; }
