
#ifndef TREE_

#define TREE_

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

Node*   tree_construct      (void)      ;
void    print_in            (Node* tree);
void    print_post          (Node* tree);
void    pro_print           (Node* node);
size_t  node_depth          (Node* root);

void    tree_destruct       (Node* m_node);

Node*   add_node            (int val,       Node* m_node);
bool    result_priority     (Node* daddy,   Node*    son);
void    node_fmt_print      (FILE* out_file,Node*   node);

void    determ_priority     (Node* node, size_t* the_priority);

//===============================================

#endif // TREE_
