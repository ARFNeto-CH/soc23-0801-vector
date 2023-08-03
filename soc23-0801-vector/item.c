#include "item.h"

#include <stdlib.h>
#include <string.h>

void* copy_book(void* book)
{
    if (book == NULL) return NULL;
    Book* one   = (Book*)book;
    Book* other = (Book*)malloc(sizeof(Book));
    if (other == NULL) return NULL;
    other->id    = one->id;
    size_t len   = 1 + strlen(one->title);
    other->title = malloc(len);
    strcpy_s(other->title, len, one->title);
    len           = 1 + strlen(one->author);
    other->author = malloc(len);
    strcpy_s(other->author, len, one->author);

    other->n_pages = one->n_pages;
    return (void*)other;
};

void* create_book(void)
{
    static size_t id         = 1000;
    char          author[40] = {0};
    char          title[40]  = {0};
    Book*         one        = (Book*)malloc(sizeof(Book));
    sprintf(title, "Title %llu", id);
    size_t len = 1 + strlen(title);
    one->title = malloc(len);
    strcpy_s(one->title, len, title);
    sprintf(author, "Author %llu", id);
    len         = 1 + strlen(author);
    one->author = malloc(len);
    strcpy_s(one->author, len, author);
    one->id      = id++;
    one->n_pages = 200 + rand() % 155;
    return (void*)one;
};

void* delete_book(void* book)
{
    Book* del = book;
    free((void*)(del->title));
    free((void*)(del->author));
    free((void*)del);
    return NULL;
}

int print_book(void* book)
{
    if (book == NULL) return 0;
    Book* one = book;
    printf(
        "\
    Book ID\t: %llu\n\
    title  \t: %s [%llu pages]\n\
    author \tt: %s\n\
\n",
        one->id, one->title, one->n_pages, one->author);
    return 0;
};

int print_book_alt(void* book)
{
    if (book == NULL) return 0;
    Book* one = (Book*)book;
    printf(
        "%8llu, \"%s [%llu pages]\", [%s]\n", one->id,
        one->title, one->n_pages, one->author);
    return 0;
}
