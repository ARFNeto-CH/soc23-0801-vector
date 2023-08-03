#include <stdio.h>
#include <stdlib.h>

#include "container.h"
#include "item.h"

int main(void)
{
    const int test_size = 8;
    srand(20230801);  // seed for same results
    Container* books =
        ctn_create(30, copy_book, delete_book, print_book);
    printf(
        "\
    Container created. empty() returned %d\n\
    Test size is %d, Capacity is %d\n",
        ctn_empty(books), test_size, ctn_capacity(books));
    for (int id = 0; id < test_size; id += 1)
    {
        Book* newb = create_book();
        ctn_push_back(newb, books);
        delete_book(newb);
    };
    ctn_show(books);
    printf(
        "\n\t***** now print again, using new layout\n\n");
    books->show = print_book_alt;
    ctn_show(books);  // now using _alt
    printf("    deleting last element\n");
    ctn_pop_back(books);
    ctn_show(books);  // now using _alt
    printf("    clearing container\n");
    ctn_clear(books);
    ctn_show(books);  // now empty
    printf("    destroying container\n");
    books = ctn_destroy(books);  // delete all
    printf("    show supposed empty container\n");
    ctn_show(books);  // now empty
    printf("    game over\n");
    return 0;
}
