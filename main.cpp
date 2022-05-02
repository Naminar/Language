
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "include/node.h"
#include "include/dump.h"
#include "include/asm.h"
#include "HashList/list.h"
#include "include/lexer.h"

//===============================================

#define DEBUG\
    printf("%s ", __PRETTY_FUNCTION__);\
    node_fmt_print(stdout, WORKING_TAPE->node);\
    printf("\n");

#define NEXT_TAPE WORKING_TAPE = WORKING_TAPE->prev

#define PREV_TAPE WORKING_TAPE = WORKING_TAPE->next

List* WORKING_TAPE = nullptr;//рабочая лента

//===============================================

const size_t SYNTAX_ERROR = 0;

/*

G Grammar
E Expression
T Term
D Degree
P Primary expression
V Variable
N Number

VU Variables using
VI Variables initialization (preliminary version)
F function
NVV new value of variables
LT letters

G::= VU | IF | WHILE 'END_OF_TOKENS'

IF::= 'if('E')'
WHILE:: = 'while('E')'

VU::= { { '@'VI';' } | { NVV';' } }*
VI::= [a-z,A-Z]+ { '='E{ ','[a-z,A-Z]+'='E } }
NVV::= [a-z,A-Z]+'='E

E::= T{ [+-]T }*
T::= D{ [*\]D }*
D::= P{ [^]P }*
P::= '('E')' | N | V
V::= [a-z,A-Z]+
N::= [0-9]+


tree builder:

tree model such as in differentiator

with lexer and parser
*/

//===============================================

Node* getG(void);
Node* getIF(void);
Node* getWHILE(void);
Node* getVU(void);

Node* getE(void);
Node* getT(void);
Node* getD(void);
Node* getP(void);
Node* getV(void);
Node* getN(void);

Node* getVI(void);
Node* getNVV(void);

Node* get_recursive_equal_sign(Node** the_last_equal_node);

//===============================================

HashTree* tree = (HashTree*) calloc(1, sizeof (HashTree));

//===============================================

typedef enum ErrorCode{

    FAILED_TYPE,
    FAILED_DATA,

    FAILED_VAR_REDECLARATION,
    FAILED_VAR_NOT_INIT

} ErrorCode;

void syntax_error_handler(List* list_of_error_node, const char* pretty_function, ErrorCode error_code,
                            Type expected_type          = EMPTY_NODE,
                            OperAndFunc expected_data   = NULL_OPER,
                            const char* var_name        = nullptr);

void syntax_error_handler(List* list_of_error_node, const char* pretty_function, ErrorCode error_code,
                            Type expected_type, OperAndFunc expected_data, const char* var_name)
{
    FILE* out_file = stdout;

    fprintf(out_file, "\n----\t ERRORS \t----\n\n");

    size_t list_ind = 0;

    List* main_list = list_of_error_node;

    while (list_of_error_node->node->type != END_OF_TOKENS
           &&
           (list_of_error_node->next->node->type != OPERATOR
              ||
              list_of_error_node->next->node->data.stat != ';')
           &&
           list_ind < 10
          )
    {
        node_fmt_print(out_file, list_of_error_node->node);

        list_of_error_node = list_of_error_node->prev;

        ++list_ind;
    }

    if (list_of_error_node->node->type == END_OF_TOKENS)
        putc('.', out_file);

    else if (list_ind == 10)
        fprintf(out_file, "...");

    fprintf(
            out_file,

            "\n ^\n"
            " ^ error here\n"
            "Line %zu, Column %zu\n\n",

            main_list->string_place,
            main_list->cursor_place
           );

    switch (error_code)
    {
        case FAILED_TYPE:
        {
            fprintf(out_file, "Expected ");

            switch (expected_type)
            {
                case VARIABLE:
                {
                    fprintf(out_file, "variable");

                    break;
                }

                case FUNCTION:
                {
                    fprintf(out_file, "system function ");

                    break;
                }

                case USER_FUNCTION:
                {
                    fprintf(out_file, "function");

                    break;
                }

                case OPERATOR:
                {
                    fprintf(out_file, "operator - %c", expected_data);

                    break;
                }

                case INT:
                {
                    fprintf(out_file, "data with int type");

                    break;
                }

                case DOT:
                {
                    fprintf(out_file, "data with dot type");

                    break;
                }

                case END_OF_TOKENS:
                {
                    fprintf(out_file, "end of program");

                    break;
                }

                default:
                {
                    fprintf(out_file, "System error!!");

                    assert (0);
                }
            }

            putc('.', out_file);

            break;
        }

        case FAILED_DATA:
        {
            if (expected_type == OPERATOR)
            {
                fprintf(out_file, "expected %c", expected_data);
            }

            break;
        }

        case FAILED_VAR_NOT_INIT:
        {
            fprintf(out_file, "variable %s didn't initialize", var_name);

            break;
        }

        case FAILED_VAR_REDECLARATION:
        {
            fprintf(out_file, "redeclaration of this variable - %s ", var_name);

            break;
        }

        default:
        {
            fprintf(out_file, "System error!! Program close.");

            assert (0);
        }
    }

    putc('\n', out_file);

    fprintf(out_file, "Signal from %s\n\n", pretty_function);

    assert (SYNTAX_ERROR);
}

int main(void)
{
    Tree* tokens_list = begin_lexering("hell.txt");

    WORKING_TAPE = tokens_list->lst->prev;

    H_list_init(tree, 5);

    Node* root = getG();

    graph_tree_dump(root);

    do_asm_translation(root);

    do_tree_simplify(&root);

    graph_tree_dump(root);

    H_list_destructor(tree);

    tree_destruct(root);

    list_destructor(tokens_list);

    free(tree);

    free(tokens_list);
}

Node* getG(void)
{
    Node* root  = nullptr,
        * daddy = nullptr;

    while ((root = getVU())
            ||
           (root = getIF())
            ||
           (root = getWHILE())
          )
    {
        daddy = new_node(EMPTY_NODE, EQUAL, root, daddy);
    }

    if (WORKING_TAPE->node->type != END_OF_TOKENS)
    {
        syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                FAILED_TYPE, END_OF_TOKENS);
    }

    return daddy;
}

Node* getIF(void)
{
    Node* if_root = nullptr;

    if (WORKING_TAPE->node->type == FUNCTION
        &&
        WORKING_TAPE->node->data.stat == FUNC_if
       )
    {
        if_root = WORKING_TAPE->node;

        daddy_and_sons_connection(WORKING_TAPE->node, new_node(FUNCTION, FUNC_then));

        NEXT_TAPE;

        if (WORKING_TAPE->node->type == OPERATOR
            &&
            WORKING_TAPE->node->data.stat == '('
           )
        {
            tree_destruct(WORKING_TAPE->node);

            NEXT_TAPE;

            if_root->left_son = getE();

            if (WORKING_TAPE->node->type == OPERATOR
                &&
                WORKING_TAPE->node->data.stat == ')'
               )
            {
                tree_destruct(WORKING_TAPE->node);

                NEXT_TAPE;
            }
            else
                assert (SYNTAX_ERROR);

        }
        else
            assert (0);
    }

    return if_root;
}

Node* getWHILE(void)
{
    Node* while_root = nullptr;

    if (WORKING_TAPE->node->type == FUNCTION
        &&
        WORKING_TAPE->node->data.stat == FUNC_while
       )
    {
        while_root = WORKING_TAPE->node;

        NEXT_TAPE;

        if (WORKING_TAPE->node->type == OPERATOR
            &&
            WORKING_TAPE->node->data.stat == '('
           )
        {
            tree_destruct(WORKING_TAPE->node);

            NEXT_TAPE;

            while_root->left_son = getE();

            if (WORKING_TAPE->node->type == OPERATOR
                &&
                WORKING_TAPE->node->data.stat == ')'
               )
            {
                tree_destruct(WORKING_TAPE->node);

                NEXT_TAPE;
            }
            else
                assert (0);

        }
        else
            assert (0);
    }

    return while_root;
}

Node* getVU(void)
{
    Node* left_son  = nullptr,
        * right_son = nullptr,
        * daddy     = nullptr;

    while (WORKING_TAPE->node->type == OPERATOR
           &&
           WORKING_TAPE->node->data.stat == '@'
           ||
           (WORKING_TAPE->prev->node->type == OPERATOR
             &&
             WORKING_TAPE->prev->node->data.stat == '='
             &&
             H_search_list_by_hash(tree,WORKING_TAPE->node->cell))
          )
    {
        DEBUG

        while (WORKING_TAPE->node->type == OPERATOR
               &&
               WORKING_TAPE->node->data.stat == '@'
              )
        {
            tree_destruct(WORKING_TAPE->node);

            NEXT_TAPE;

            daddy = new_node(EMPTY_NODE, EQUAL, getVI(), daddy);

            if (WORKING_TAPE->node->type != OPERATOR
                ||
                WORKING_TAPE->node->data.stat != ';'
               )
                assert (SYNTAX_ERROR);
            else
                tree_destruct(WORKING_TAPE->node);

            NEXT_TAPE;
        }

        while (H_search_list_by_hash(tree,WORKING_TAPE->node->cell)
               &&
               WORKING_TAPE->prev->node->type == OPERATOR
               &&
               WORKING_TAPE->prev->node->data.stat == '='
              )
        {
            DEBUG

            daddy = new_node(EMPTY_NODE, EQUAL, getNVV(), daddy);

            if (WORKING_TAPE->node->type != OPERATOR
                ||
                WORKING_TAPE->node->data.stat != ';'
               )
                assert (SYNTAX_ERROR);
            else
                tree_destruct(WORKING_TAPE->node);

            NEXT_TAPE;
        }
    }

    return daddy;
}

Node* getVI(void)
{
    Node* left_son  = nullptr,
        * right_son = nullptr,
        * daddy     = nullptr;

    char* variable_name = nullptr;

    DEBUG

    if (WORKING_TAPE->node->type == VARIABLE
        &&
        (WORKING_TAPE->prev->node->type == OPERATOR && WORKING_TAPE->prev->node->data.stat == ';')
       )
    {
        if (H_search_list_by_hash(tree, WORKING_TAPE->node->cell))
        {
            printf("REDEFINITION OF VARIABLE");

            assert (SYNTAX_ERROR);
        }
        else
        {
            H_list_insert(tree, 0, WORKING_TAPE->node->cell);
        }

        tree->lst->next->value.integer = 0;

        daddy = new_node(OPERATOR, EQUAL, new_node(INT), WORKING_TAPE->node);

        daddy->right_son->data.i_num = 0;

        NEXT_TAPE;

        return daddy;
    }


    do
    {
        if (WORKING_TAPE->next->node->type == OPERATOR
            &&
            WORKING_TAPE->next->node->data.stat == ','
           )
            tree_destruct(WORKING_TAPE->next->node);

        right_son = get_recursive_equal_sign(&left_son);

        daddy = new_node(EMPTY_NODE, EQUAL, right_son, daddy);

    } while (WORKING_TAPE->node->type == OPERATOR
             &&
             WORKING_TAPE->node->data.stat == ','
             &&
             (NEXT_TAPE)
            );

    return daddy;
}

Node* get_recursive_equal_sign(Node** the_last_equal_node)
{
    DEBUG

    if (WORKING_TAPE->node->type == VARIABLE
        &&
        WORKING_TAPE->prev->node->type == OPERATOR
        &&
        WORKING_TAPE->prev->node->data.stat == '='
       )
    {
        Node* left_son  = nullptr,
            * right_son = nullptr,
            * daddy     = nullptr;

        Node* a_last_equal_node = nullptr;

        char* variable_name = WORKING_TAPE->node->cell;

        if (H_search_list_by_hash(tree, variable_name))
        {
            printf("REDEFINITION OF VARIABLE");

            assert (SYNTAX_ERROR);
        }
        else
        {
            H_list_insert(tree, 0, variable_name);
        }

        left_son = WORKING_TAPE->node;

        NEXT_TAPE;

        daddy = WORKING_TAPE->node;

        NEXT_TAPE;

        right_son = get_recursive_equal_sign(&a_last_equal_node);

        if ((right_son->type == EMPTY_NODE)
            ||
            (right_son->type == OPERATOR
              &&
              right_son->data.stat == '=')
           )
        {
            daddy->right_son = a_last_equal_node;

            daddy->left_son = left_son;

            daddy = new_node(EMPTY_NODE, EQUAL, daddy, right_son);

            *the_last_equal_node = node_cpy(a_last_equal_node);
        }
        else
        {
            *the_last_equal_node = node_cpy(left_son);

            daddy_and_sons_connection(daddy, right_son, left_son);
        }

        return daddy;
    }
    else
    {
        Node* daddy = WORKING_TAPE->node;

        NEXT_TAPE;

        return daddy;
    }
}

Node* getNVV(void)
{
    Node* left_son  = nullptr,
        * daddy     = nullptr;

    if (WORKING_TAPE->node->type == VARIABLE)
    {
        HashList* detected_variable
          = H_search_list_by_hash(tree, WORKING_TAPE->node->cell);

        left_son = WORKING_TAPE->node;

        NEXT_TAPE;

        if (detected_variable
            &&
            WORKING_TAPE->node->type == OPERATOR
            &&
            WORKING_TAPE->node->data.stat == '='
           )
        {
            daddy = WORKING_TAPE->node;

            NEXT_TAPE;

            daddy_and_sons_connection(daddy, getE(), left_son);
        }
        else
        {
            printf("VARIABLE OR SIGN '=' DIDN'T FIND!!");

            assert (SYNTAX_ERROR);
        }
    }

    return daddy;
}

Node* getE(void)
{
    Node* left_son  = nullptr,
        * right_son = nullptr,
        * daddy     = nullptr;

    daddy = getT();

    DEBUG

    while (WORKING_TAPE->node->type == OPERATOR
           &&
           (WORKING_TAPE->node->data.stat == '+' || WORKING_TAPE->node->data.stat == '-'))
    {
        left_son = daddy;

        daddy = WORKING_TAPE->node;

        NEXT_TAPE;

        right_son = getT();

        daddy_and_sons_connection(daddy, right_son, left_son);
    }

    return daddy;
}

Node* getT(void)
{
    Node* left_son  = nullptr,
        * right_son = nullptr,
        * daddy     = nullptr;

    daddy = getD();

    DEBUG

    while (WORKING_TAPE->node->type == OPERATOR
           &&
           (WORKING_TAPE->node->data.stat == '*' || WORKING_TAPE->node->data.stat == '/'))
    {
        left_son = daddy;

        daddy = WORKING_TAPE->node;

        NEXT_TAPE;

        right_son = getD();

        daddy_and_sons_connection(daddy, right_son, left_son);
    }

    return daddy;
}

Node* getD(void)
{
    Node* left_son  = nullptr,
        * right_son = nullptr,
        * daddy     = nullptr;

    DEBUG

    daddy = getP();

    while (WORKING_TAPE->node->type == OPERATOR
           &&
           WORKING_TAPE->node->data.stat == '^'
          )
    {
        left_son = daddy;

        daddy = WORKING_TAPE->node;

        NEXT_TAPE;

        right_son = getP();

        daddy_and_sons_connection(daddy, right_son, left_son);
    }

    return daddy;
}

Node* getP(void)
{
    Node* recognized_node = nullptr;

    DEBUG

    if (WORKING_TAPE->node->type == OPERATOR
        &&
        WORKING_TAPE->node->data.stat == '('
       )
    {
        tree_destruct(WORKING_TAPE->node);

        NEXT_TAPE;

        recognized_node = getE();

        if (WORKING_TAPE->node->type == OPERATOR
            &&
            WORKING_TAPE->node->data.stat == ')'
           )
        {
            tree_destruct(WORKING_TAPE->node);

            NEXT_TAPE;
        }
        else
        {
            printf("\n SYNTAX ERROR!!");

            assert (0);
        }
    }
    else if (WORKING_TAPE->node->type == INT)
        recognized_node = getN();
    else
        recognized_node = getV();

    return recognized_node;
}

Node* getV(void)
{
    Node*       recognized_node   = nullptr;
    HashList*   detected_variable = nullptr;

    DEBUG

    if (WORKING_TAPE->node->type == VARIABLE)
    {
        detected_variable = H_search_list_by_hash(tree, WORKING_TAPE->node->cell);

        if (!detected_variable)
        {
            fprintf(stdout, "THIS VARIABLE DIDN'T FOUND:\"%s\"\n", WORKING_TAPE->node->cell);

            assert(detected_variable);
        }

        recognized_node = WORKING_TAPE->node;

        NEXT_TAPE;
    }

    return recognized_node;
}

Node* getN(void)
{
    Node* recognized_node = nullptr;

    DEBUG

    if (WORKING_TAPE->node->type == INT
        ||
        WORKING_TAPE->node->type == DOT
       )
    {
        recognized_node = WORKING_TAPE->node;

        NEXT_TAPE;
    }

    return recognized_node;
}

