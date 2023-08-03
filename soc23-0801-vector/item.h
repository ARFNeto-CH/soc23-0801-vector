#pragma once
#include <stdio.h>

typedef struct
{                   // struct data
    size_t id;      // book id
    char*  title;   // book title
    char*  author;  // book author
    size_t n_pages;   
} Book;             // a single book

int   print_book(void*);
int   print_book_alt(void*);  // one liner
void* copy_book(void*);
void* create_book(void);
void* delete_book(void*);
