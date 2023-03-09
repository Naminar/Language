
#ifndef HASH_LIST_

#define HASH_LIST_

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "../include/node.h"

//===============================================

typedef union VarValue{

    int integer;

    double dot;

} VarValue;

typedef enum VarType{

    V_VARIABLE, V_MATRIX, V_LIST, V_USER_FUNCTION

} VarType;

typedef struct HashList{

    VarType var_type;

    char* var_name      = nullptr;

    size_t var_value    = 0;

    size_t hash         = 0;

    size_t ram_place;

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
HashList*   H_search_list_by_hash   (HashTree* tree, char* a_var_name);
bool        H_list_insert           (HashTree* tree, size_t num, char* a_var_name, VarType a_var_type);

#endif // HASH_LIST_
