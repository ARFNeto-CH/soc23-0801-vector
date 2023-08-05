#include <stdio.h>
#include <stdlib.h>

#include "container.h"
#include "item.h"

int main(void)
{
    const int test_size = 80;
    srand(20230801);  // seed for same results
    Container* ctn =
        ctn_create(test_size, copy_int, delete_int, print_int);
    printf(
        "\
    Container created. empty() returned %d\n\
    Test size is %d, Capacity is %d\n",
        ctn_empty(ctn), test_size, ctn_capacity(ctn));
    for (int id = 0; id < test_size; id += 1)
    {
        int* newb = create_int();
        ctn_push_back(newb, ctn);
        delete_int(newb);
    };
    ctn_show(ctn);
    printf("    deleting last element\n");
    ctn_pop_back(ctn);
    ctn_show(ctn);  // now using _alt
    printf("    clearing container\n");
    ctn_clear(ctn);
    ctn_show(ctn);  // now empty
    printf("    destroying container\n");
    ctn = ctn_destroy(ctn);  // delete all
    printf("    show supposed empty container\n");
    ctn_show(ctn);  // now empty
    printf("    game over\n");
    return 0;
}
