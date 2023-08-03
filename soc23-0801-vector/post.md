This is the `Header` and prototypes    

```C
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

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

void  _arr_add(void*, void*);
void* _arr_create(size_t);
```

And to understand the idea here --- and in the [original article](https://solarianprogrammer.com/2017/01/08/c99-c11-dynamic-array-mimics-cpp-vector-api-improvements/) --- we need to look at the `ARRAY_CREATE` macro

```C
#define ARRAY_CREATE(T, arr)                               \
    T* arr = NULL;                                         \
    do {                                                   \
        size_t* raw = malloc(2 * sizeof(size_t));          \
        raw[0]      = 0;                                   \
        raw[1]      = 0;                                   \
        arr         = (void*)&raw[2];                      \
    } while (0)
```

**In short:** the idea is to set an array of pointers in `arr`. Some initial elements are saved for use as metadata, like `size` and `capacity`. And a new `array` is used, pointing to just after the metadata, as some form of encapsulation.

**The stated target:** create a *dynamic array that mimics C++'s std::vector* . And the version in the article is an improvement over a previous one. 

### Why your code fails ###    

When adding the function `_arr_add` when you change `arr` or `num` inside the changes are local and are lost on return, since `_arr_add` returns nothing. Then `realloc` is called and the new address is lost right away.

### Why this idea is not so good anyway ###    

  - Use of macros to hide details
  - obfuscate metadata of the data structure
  - ignore the nature of the data to be stored in the array
  
  ... everything goes in the direction of problems.

  If the idea is to *mimic C++ std::vector* as said by the article author, why not consider the C++ methods (functions)? Could they be wrong? Why not look at the [unofficial manual of C++ vector](https://en.cppreference.com/w/cpp/container/vector) for ideas?

  So
   - there is a function *constructor* to create a vector and return its address
   - there is a destructor to delete a vector
   - `size`,`empty`,`capacity` returns the expected info
   - `at` return element at position
   - `push_back` add an element to the end
   - `clear` deletes all elements
   - `pop_back` removes the last element

   ### problems with the article's idea ###    
 
- not mentioned in the article: if the type 'T' allocates memory there is no way to free it.
- the data is not copied. There is no way to do this. If the user pass dynamic data to `ARRAY_PUSH` there is nothing to save the code from crashing if the owner frees the data elsewhere.

I would also point that the code is hard to read, hard to mantain, and (in the code posted by the OP) the idea of use functions with the name differing by an `_` from the macros is not great.

```C
#define arr_create(type) _arr_create(sizeof(type))
#define arr_add(arr, num) _arr_add(arr, &num)

void  _arr_add(void*, void*);
void* _arr_create(size_t);
```

## `C` containers: a possible approach (with implementation and example) ##    
In `C++`, `vector` is called a `container`. In `java` and other languages it is called a `Collection`. Sets, maps, lists, queues, stacks and arrays are also collections implemented in those languages.
I will show here an abstraction for the `C++` vector, with the `C` code and tests. It is more like a `C++` `std:array`     

Not production code, just a simple thing to build upon.

**But it is much more powerful and resilient than the original article.**

### The general idea ###    

- The container abstraction is an array of pointers to the contents, here called `item`.
- Since an `item` can allocate memory and, as we need something generic, no data is kept from the data provided by the user when adding data: everything is copied.
- as is common technique in `C` since ever, as in `qsort` of `C` standard library, the user of the container supllies a way to copy, destroy and show an `item`. These are like *copy constructor*, *destructor* and *`<<` overlay* in `C++`.    

### `Container.h` example ###    

A container is a generic struct and makes no reference to its contents

```C
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    size_t limit;             // capacity
    size_t size;              // actual size
    void** item;              // an array of pointers to items
    void* (*copy)(void*);     // copy
    void* (*destroy)(void*);  // destructor
    int (*show)(void*);       // print
} Container;                  // a collection

int        ctn_capacity(Container*);
int        ctn_clear(Container*);
Container* ctn_create(
    size_t, void* (*copy)(void*), void* (*destroy)(void*),
    int (*show)(void*));
Container* ctn_destroy(Container*);
int        ctn_empty(Container*);
int        ctn_pop_back(Container*);
int        ctn_push_back(void*, Container*);
int        ctn_resize(Container*, const size_t);
int        ctn_show(Container*);
int        ctn_size(Container*);
```

Here we see some of the `C++` methods (functions) for vectors. `ctn_create` is like the `constructor` in the `C++` sense:

```C
Container* ctn_create(
    size_t, void* (*copy)(void*), void* (*destroy)(void*),
    int (*show)(void*));
``` 

Here a simple call as in the example below:    

```C
    Container* books =
        ctn_create(30, copy_book, delete_book, print_book);
```

A container called `books` is being created. `30` is the size limit, and the names of 3 functions follows. They are used to copy, delete and show items, so the `Container` does not need to know about its own contents.

### A simple implementation for `Container.c` ###    

```C
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
```

## Building an example ##    

### An `item` for the container: `item.h` ###    

```C
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
``` 

This has no special meaning. It is just for feeding something to the container.    
Note that there are 2 versions of `print_book` to show how to **overload functions in the same container** :). Each book has title, author, id and #of pages.

### a simple implementation for `item.c` ###    

```C
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
```

### `main` for a simple test ###    

```C
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
```

A container is created and a few tests are run.

### output of the test ###    

```none
    Container created. empty() returned 1
    Test size is 8, Capacity is 30
    there are 8 of 30 items

    Book ID     : 1000
    title       : Title 1000 [274 pages]
    author      t: Author 1000

    Book ID     : 1001
    title       : Title 1001 [284 pages]
    author      t: Author 1001

    Book ID     : 1002
    title       : Title 1002 [248 pages]
    author      t: Author 1002

    Book ID     : 1003
    title       : Title 1003 [259 pages]
    author      t: Author 1003

    Book ID     : 1004
    title       : Title 1004 [351 pages]
    author      t: Author 1004

    Book ID     : 1005
    title       : Title 1005 [228 pages]
    author      t: Author 1005

    Book ID     : 1006
    title       : Title 1006 [334 pages]
    author      t: Author 1006

    Book ID     : 1007
    title       : Title 1007 [342 pages]
    author      t: Author 1007

[end of listing]

        ***** now print again, using new layout

    there are 8 of 30 items

    1000, "Title 1000 [274 pages]", [Author 1000]
    1001, "Title 1001 [284 pages]", [Author 1001]
    1002, "Title 1002 [248 pages]", [Author 1002]
    1003, "Title 1003 [259 pages]", [Author 1003]
    1004, "Title 1004 [351 pages]", [Author 1004]
    1005, "Title 1005 [228 pages]", [Author 1005]
    1006, "Title 1006 [334 pages]", [Author 1006]
    1007, "Title 1007 [342 pages]", [Author 1007]
[end of listing]
    deleting last element
    pop_back(): size is now 7
    there are 7 of 30 items

    1000, "Title 1000 [274 pages]", [Author 1000]
    1001, "Title 1001 [284 pages]", [Author 1001]
    1002, "Title 1002 [248 pages]", [Author 1002]
    1003, "Title 1003 [259 pages]", [Author 1003]
    1004, "Title 1004 [351 pages]", [Author 1004]
    1005, "Title 1005 [228 pages]", [Author 1005]
    1006, "Title 1006 [334 pages]", [Author 1006]
[end of listing]
    clearing container
    there are 0 of 30 items

[end of listing]
    destroying container
    show supposed empty container
    game over
``` 

This is a simple way to implement any type of container. Just add the item definitions, write the functions and go.

To run just





