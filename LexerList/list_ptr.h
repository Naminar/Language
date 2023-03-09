
#ifndef LIST_
#define LIST_

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#include "../include/node.h"

//===============================================

typedef struct List{

    Node* node;

    List* next;
    List* prev;

    size_t cursor_place = 0;
    size_t string_place = 0;

} List;

typedef struct Tree{

    List* lst      ;

    size_t capacity;

    size_t size    ;

} Tree;

//===============================================

void    list_destructor         (Tree*);
void    list_dump               (Tree*);


void    list_init               (Tree*, size_t);
bool    list_delete_after       (Tree*, size_t);
List*   search_element          (Tree*, size_t);

bool    list_insert             (Tree* tree, size_t num, Node* a_node,
                                 size_t a_cursor_place, size_t a_string_place);

#endif // LIST_
