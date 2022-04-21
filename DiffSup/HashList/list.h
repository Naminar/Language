
#ifndef HASH_LIST_

#define HASH_LIST_

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "../include/tree.h"

//===============================================

typedef const size_t hash_type;

typedef struct HashList{

    char symbol = 0;

    size_t frequency = 0;

    size_t hash = 0;

    Node* node;

    HashList* next;

    HashList* prev;

} HashList;

typedef struct HashTree{

    HashList* lst       ;

    size_t capacity     ;

    size_t size         ;

} HashTree;

//===============================================

void        H_list_destructor       (HashTree* tree);
bool        H_list_delete_after     (HashTree* tree, size_t num);
void        H_list_init             (HashTree* tree, size_t capacity);
HashList*   H_search_list_by_hash   (HashTree* tree, hash_type a_hash);
bool        H_list_insert           (HashTree* tree, size_t num,
                                     hash_type a_hash, Node* first_node = nullptr);

#endif // HASH_LIST_
