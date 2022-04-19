
#ifndef NODE_
#define NODE_

#include <stdlib.h>
#include <cstdio>
#include <assert.h>
#include "config.h"

#define CELL_FMT "%s"

//===============================================

const size_t ConstHash = 0XA12C78D;

//===============================================

typedef enum OperAndFunc{

    NULL_OPER = -111,

    #define newoper(name, val, codir,...)\
        name = codir,

    #define newfun(name, codir, ...)\
        FUNC_##name = codir,

    #include "../function"

    #undef newfun
    #undef newoper

} OperAndFunc;

typedef enum Type{

    INT         = 'i',
    DOT         = 'd',
    VARIABLE    = 'v',
    FUNCTION    = 'f',
    OPERATOR    = 'o'

} Type;

typedef union Data{

    int i_num;

    double d_num;

    OperAndFunc stat; //statement

} Data;

typedef struct Node{

    char* cell;

    Type type;
    Data data;

    size_t hash = 0;

    Node* left_son;
    Node* right_son;

} Node;

//===============================================

void node_fmt_print(FILE* out_file, Node* node);
void pro_print(Node* node);
void tree_destruct(Node* node);
Node* tree_construct(void);
Node* new_node(Type a_type, OperAndFunc a_stat, Node* right_node, Node* left_node);

//===============================================

#endif // NODE_
