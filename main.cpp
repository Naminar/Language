
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

#define DEBUG                                   \
    printf("%s ", __PRETTY_FUNCTION__);         \
    node_fmt_print(stdout, WORKING_TAPE->node); \
    printf("\n");

#define NEXT_TAPE WORKING_TAPE = WORKING_TAPE->prev

#define PREV_TAPE WORKING_TAPE = WORKING_TAPE->next

List* WORKING_TAPE = nullptr;//������� �����

//===============================================

const size_t SYNTAX_ERROR = 0,
             SYSTEM_ERROR = 0;

/* The matrix is not implemented

G Grammar
E Expression
T Term
D Degree
P Primary expression
COMP compare
V Variable
N Number

VU Variables using
VI Variables initialization (preliminary version)
F function
NVV new value of variables
LT letters

getFuncInit::= 'function' name_of_function '(' getFuncArguments ')' '{' G '}'
             { 'function' name_of_function '(' getFuncArguments ')' '{' G '}'
             }*

getFuncArguments::= [a-z,A-Z]+ { ' ' [a-z,A-Z]+ }*

G::= { VU | IF | WHILE | MLU }* 'END_OF_TOKENS'

VU::= { { '@'VI';' } | { NVV';' } }*
IF::= 'if(' E ')true{' G '}'  // then
WHILE:: = 'while('E')'
MLU:: =  let [a-z,A-Z]+ { ,[a-z,A-Z]+}* as int MRX | LST { and MLU[without let]}*   //int | dot

MRX:: = matrix (N * N)={N{,N}*}
LST:: = list   (N)    ={N{,N}*}

VI::= [a-z,A-Z]+ { '='VI | E{ ','VI }* }*
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
typedef struct FuncParameters{

    const char* func_name   = nullptr;

    size_t func_length      = 0;

} FuncParameters;

char* create_shadow_string(const char* cell_name, const FuncParameters* func_param);

char* capsule_fusioning(char** cells_name, const size_t cells_length,
                        const FuncParameters* func_param);

Node* getFuncArguments(int* arg_value, const FuncParameters* func_param);
Node* getFuncInit(void);
Node* getSHOW(const FuncParameters* func_param);
Node* getG(const FuncParameters* func_param);
Node* getIF(const FuncParameters* func_param);
Node* getWHILE(const FuncParameters* func_param);
Node* getVU(const FuncParameters* func_param);
Node* getFU(const FuncParameters* func_param);

Node* getMLU(const FuncParameters* func_param);
Node* getMRX(void);
Node* getLST(const char* l_name, int* list_length,
                const FuncParameters* func_param, int* number_of_items_list);

Node* getCOMP(const FuncParameters* func_param);
Node* getE(const FuncParameters* func_param);
Node* getT(const FuncParameters* func_param);
Node* getD(const FuncParameters* func_param);
Node* getP(const FuncParameters* func_param);
Node* getMF();
Node* getLI(const FuncParameters* func_param);
Node* getV(const FuncParameters* func_param);
Node* getN(void);

Node* getVI(const FuncParameters* func_param);
Node* getNVV(const FuncParameters* func_param);

Node* get_recursive_equal_sign(Node** the_last_equal_node, const FuncParameters* func_param);
Node* get_recursive_FUNC_as(Node** the_last_equal_node, int*, const FuncParameters* func_param, int* number_of_items_list);
//===============================================

HashTree* tree = (HashTree*) calloc(1, sizeof (HashTree));

//===============================================

typedef enum ErrorCode{

    FAILED_TYPE,
    FAILED_DATA,

    FAILED_VAR_REDECLARATION,
    FAILED_VAR_NOT_INIT,

    FAILED_FUNCTION_REDECLARATION,
    FAILED_FUNCTION_NOT_INIT,
    FAILED_FUNCTION_ARG_NUMBER,

    FAILED_NOT_SINGLE_FUNCTION,

    FAILED_ANOTHER

} ErrorCode;

void syntax_error_handler(List* list_of_error_node, const char* pretty_function, ErrorCode error_code,
                            Type expected_type          = EMPTY_NODE,
                            OperAndFunc expected_data   = NULL_OPER);

Node* create_list_initialization(const char* variable_name, const char* last_variable_name,
                                    int arg_length, const FuncParameters* func_param);

Node* init_variable_node_with_index_name(Node* node, const char* name, int number, const FuncParameters* func_param);

void syntax_error_handler(List* list_of_error_node, const char* pretty_function, ErrorCode error_code,
                            Type expected_type, OperAndFunc expected_data)
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
    {
        putc('.', out_file);
    }
    else if (list_ind == 10
             &&
             list_of_error_node->next->node->data.stat != ';'
            )
    {
        fprintf(out_file, "...");
    }

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

                case CALL_USER_FUNCTION:
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

                    assert (SYSTEM_ERROR);
                }
            }

            //putc('.', out_file);

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
            fprintf(out_file, "variable %s didn't initialize", main_list->node->cell);

            break;
        }

        case FAILED_VAR_REDECLARATION:
        {
            fprintf(out_file, "redeclaration of this variable - %s ", main_list->node->cell);

            break;
        }

        case FAILED_FUNCTION_REDECLARATION:
        {
            fprintf(out_file, "redeclaration of this function - %s ", main_list->node->cell);

            break;
        }

        case FAILED_FUNCTION_NOT_INIT:
        {
            fprintf(out_file, "this function didn't initialize - %s ", main_list->node->cell);

            break;
        }

        case FAILED_FUNCTION_ARG_NUMBER:
        {
            fprintf(out_file, "invalid number of arguments passed here");

            break;
        }

        case FAILED_ANOTHER:
        {
            fprintf(out_file, "don't understand the meaning");

            break;
        }

        case FAILED_NOT_SINGLE_FUNCTION:
        {
            fprintf(out_file, "There is no first function in this file");

            break;
        }

        default:
        {
            fprintf(out_file, "System error!! Program close.");

            assert (SYSTEM_ERROR);
        }
    }

    putc('.', out_file); //from 234 line

    putc('\n', out_file);

    fprintf(out_file, "Signal from %s\n\n", pretty_function);

    assert (SYNTAX_ERROR);
}

char* my_itoa(int num, char* buffer, int base)
{
    int curr = 0;

    if (num == 0)
    {
        buffer[curr++] = '0';
        buffer[curr]   = '\0';

        return buffer;
    }

    int num_digits = 0;

    if (num < 0)
    {
        if (base == 10)
        {
            num_digits ++;

            buffer[curr] = '-';

            curr ++;

            // Make it positive and finally add the minus sign
            num *= -1;
        }
        else
            // Unsupported base. Return NULL
            return NULL;
    }

    num_digits += (int)floor(log(num) / log(base)) + 1;

    // Go through the digits one by one
    // from left to right
    while (curr < num_digits)
    {
        // Get the base value. For example, 10^2 = 1000, for the third digit
        int base_val = (int) pow(base, num_digits-1-curr);

        // Get the numerical value
        int num_val = num / base_val;

        char value = num_val + '0';
        buffer[curr] = value;

        curr ++;
        num -= base_val * num_val;
    }

    buffer[curr] = '\0';

    return buffer;
}

int main(void)
{
    Tree* tokens_list = begin_lexering("hell.txt");

    WORKING_TAPE = tokens_list->lst->prev;

    H_list_init(tree, 30);

    Node* root = getFuncInit(); //getG();

    //graph_tree_dump(root);

    do_tree_simplify(&root);

    graph_tree_dump(root);

    do_asm_translation(root);

    H_list_destructor(tree);

    tree_destruct(root);

    /*printf("%d", tokens_list->size);

    List* lll = tokens_list->lst;

    for (int i = 0; i < tokens_list->size; i++)
    {
        node_fmt_print(stdout, lll->prev->node);

        lll = lll->prev;
    }*/

    list_destructor(tokens_list);

    free(tree);

    free(tokens_list);
}

Node* getG(const FuncParameters* func_param)
{
    Node* root  = nullptr,
        * daddy = nullptr;

    while ((root = getVU(func_param))
           ||
           (root = getIF(func_param))
           ||
           (root = getWHILE(func_param))
           ||
           (root = getSHOW(func_param))
           ||
           (root = getMLU(func_param))
           ||
           (root = getFU(func_param))
          )
    {
        daddy = new_node(EMPTY_NODE, EQUAL, root, daddy);
    }

    /*if (WORKING_TAPE->node->type != END_OF_TOKENS)
    {
        syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                FAILED_TYPE, END_OF_TOKENS);
    }*/


    /*printf("&&&&&&&");
    node_fmt_print(stdout, WORKING_TAPE->node);*/

    return daddy;
}

Node* getSHOW(const FuncParameters* func_param)
{
    Node* daddy = nullptr;

    DEBUG

    if (WORKING_TAPE->node->type == FUNCTION
        &&
        WORKING_TAPE->node->data.stat == FUNC_show
       )
    {
        daddy = WORKING_TAPE->node;

        NEXT_TAPE;

        if (WORKING_TAPE->node->type == FUNCTION
            &&
            WORKING_TAPE->node->data.stat == FUNC_this
           )
        {
            NEXT_TAPE;

            if (daddy->right_son = getV(func_param)){}
            else
                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                        FAILED_TYPE, VARIABLE);
        }
        else
        {
            if (daddy->right_son = getN()){}
            else
                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                        FAILED_TYPE, INT);
        }

        if (WORKING_TAPE->node->type == OPERATOR
            &&
            WORKING_TAPE->node->data.stat == SEMICOLON
           )
            NEXT_TAPE;
        else
            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                    FAILED_TYPE, OPERATOR, SEMICOLON);
    }

    return daddy;
}

Node* getMLU(const FuncParameters* func_param)
{
    Node* left_son  = nullptr,
        * right_son = nullptr,
        * daddy     = nullptr;

    DEBUG

    if (WORKING_TAPE->node->type == FUNCTION
        &&
        WORKING_TAPE->node->data.stat == FUNC_let
       )
    {
        //tree_destruct(WORKING_TAPE->node);

        NEXT_TAPE;

        if (WORKING_TAPE->node->type != VARIABLE)
            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__, FAILED_TYPE, VARIABLE);

        int help_int, number_of_items_list;

        daddy = get_recursive_FUNC_as(&right_son, &help_int, func_param, &number_of_items_list);

    }

    return daddy;
}

Node* get_recursive_FUNC_as(Node** the_last_equal_node, int* arg_length,
                                const FuncParameters* func_param, int* number_of_items_list)
{
    DEBUG

    Node    * left_son  = nullptr,
            * right_son = nullptr,
            * daddy     = nullptr;

    if (WORKING_TAPE->node->type == VARIABLE
        &&
        WORKING_TAPE->prev->node->type == FUNCTION
        &&
        WORKING_TAPE->prev->node->data.stat == FUNC_as
       )
    {
        Node* a_last_equal_node = nullptr;

        char* without_postfix_variable_name = nullptr;

        size_t variable_length = strlen(WORKING_TAPE->node->cell);

        without_postfix_variable_name = (char*) calloc(sizeof(char), variable_length + 1);

        assert(without_postfix_variable_name);

        strcpy(without_postfix_variable_name, WORKING_TAPE->node->cell);

        //
        //printf("_____%s______", without_postfix_variable_name);
        //
        capsule_fusioning(&WORKING_TAPE->node->cell,
                                variable_length, func_param);

        if (H_search_list_by_hash(tree, WORKING_TAPE->node->cell))
        {
            //printf("REDEFINITION OF VARIABLE");

            //assert (SYNTAX_ERROR);

            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                    FAILED_VAR_REDECLARATION);
        }
        /*else
        {
            H_list_insert(tree, 0, WORKING_TAPE->node->cell, V_VARIABLE);
        }*/

        left_son = WORKING_TAPE->node;

        NEXT_TAPE;

        daddy = WORKING_TAPE->node;

        NEXT_TAPE;

        right_son = get_recursive_FUNC_as(&a_last_equal_node, arg_length, func_param, number_of_items_list);

        //printf("%s === %d", left_son->cell, *number_of_items_list); ///

        H_list_insert(tree, 0, left_son->cell, V_VARIABLE);

        tree->lst->next->var_value = *number_of_items_list; //added the value of inserted list like number_of_items_list

        //printf("%s === %d\n", tree->lst->next->var_name, tree->lst->next->var_value);

        if ((right_son->type == EMPTY_NODE)
            ||
            (right_son->type == FUNCTION
              &&
              right_son->data.stat == FUNC_as)
           )
        {
            daddy->right_son = a_last_equal_node;

            daddy->left_son = left_son;

            daddy = new_node(EMPTY_NODE, EQUAL, daddy, right_son);

            daddy->right_son->right_son->left_son
            = create_list_initialization(without_postfix_variable_name, a_last_equal_node->cell,
                                            *arg_length, func_param);

            *the_last_equal_node = node_cpy(a_last_equal_node);
        }
        else
        {
            *the_last_equal_node = node_cpy(left_son);

            daddy_and_sons_connection(daddy, right_son, left_son);
        }

        free(without_postfix_variable_name);

        return daddy;
    }
    else
    {
        if (WORKING_TAPE->node->type == FUNCTION
            &&
            WORKING_TAPE->node->data.stat == FUNC_int
           )
        {
            //tree_destruct(WORKING_TAPE->node);

            NEXT_TAPE;

            if (!(right_son = getMRX())
                &&
                !(right_son = getLST(WORKING_TAPE->next->next->next->node->cell,
                                        arg_length, func_param, number_of_items_list)
                 )
               )
                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__, FAILED_TYPE, FUNCTION, FUNC_list);
        }
        else
            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                    FAILED_TYPE, FUNCTION);

        return right_son;
    }
}

Node* create_list_initialization(const char* l_name, const char* last_l_name,
                                    int arg_length, const FuncParameters* func_param)
{
    Node* equal_daddy       = nullptr,
        * equal_left_son    = nullptr,
        * equal_right_son   = nullptr,
        * daddy             = nullptr;

    char buffer[17];

    size_t l_length = strlen (l_name),
           b_length = 0;

    for (size_t l_ind = 0; l_ind < arg_length; l_ind++)
    {
        equal_left_son = init_variable_node_with_index_name(new_node(VARIABLE), l_name, l_ind, func_param);

        equal_right_son = init_variable_node_with_index_name(new_node(VARIABLE), last_l_name, l_ind, func_param);

        H_list_insert(tree, 0, equal_left_son->cell, V_VARIABLE);


        equal_daddy = new_node(OPERATOR, EQUAL, equal_right_son, equal_left_son);

        daddy = new_node(EMPTY_NODE, EQUAL, equal_daddy, daddy);
    }

    return daddy;
}

Node* init_variable_node_with_index_name(Node* node, const char* name, int number, const FuncParameters* func_param)
{
    char buffer[17] = {};

    size_t name_length = strlen (name);

    my_itoa(number, buffer, 10);

    size_t b_length = strlen (buffer);

    node->cell = (char*) realloc(node->cell, sizeof (char) * (name_length + b_length + 2));

    assert (name);

    strcpy(node->cell, name);

    node->cell[name_length] = '_';

    strncpy(&node->cell[name_length + 1], buffer, b_length);

    node->cell[b_length + name_length + 1] = '\0';

    //printf("_____%s______", node->cell);

    capsule_fusioning(&node->cell, strlen(node->cell), func_param);

    return node;
}

Node* getMRX(void)
{
    Node* daddy = nullptr;

    DEBUG

    /*if (WORKING_TAPE->node->type == FUNCTION
        &&
        WORKING_TAPE->node->data.stat == FUNC_matrix
       )
    {
        daddy = WORKING_TAPE->node;

        NEXT_TAPE;

        if (WORKING_TAPE->node->type == OPERATOR
            &&
            WORKING_TAPE->node->data.stat == '('
           )
        {
            tree_destruct(WORKING_TAPE->node);

            NEXT_TAPE;



            //if (getN());
        }

        //
    }*/

    return daddy;
}

Node* getLST(const char* l_name, int* list_length,
                const FuncParameters* func_param, int* number_of_items_list)
{
    Node* left_son  = nullptr,
        * arg_node  = nullptr,
        * daddy     = nullptr;

    DEBUG

    if (WORKING_TAPE->node->type == FUNCTION
        &&
        WORKING_TAPE->node->data.stat == FUNC_list
       )
    {
        daddy = WORKING_TAPE->node;

        NEXT_TAPE;

        if (WORKING_TAPE->node->type == OPERATOR
            &&
            WORKING_TAPE->node->data.stat == '('
           )
        {
            //tree_destruct(WORKING_TAPE->node);

            NEXT_TAPE;

            if ((arg_node = getN())
                &&
                arg_node->type == INT
                &&
                arg_node->data.i_num >= 0
                &&
                (*number_of_items_list = arg_node->data.i_num)
               )
            {
                *list_length = arg_node->data.i_num;

                tree_destruct(arg_node);

                if (WORKING_TAPE->node->type == OPERATOR
                    &&
                    WORKING_TAPE->node->data.stat == ')'
                   )
                {
                    //tree_destruct(WORKING_TAPE->node);

                    NEXT_TAPE;

                    if (WORKING_TAPE->node->type == OPERATOR
                        &&
                        WORKING_TAPE->node->data.stat == '='
                       )
                    {
                        //tree_destruct(WORKING_TAPE->node);

                        NEXT_TAPE;

                        if (WORKING_TAPE->node->type == OPERATOR
                            &&
                            WORKING_TAPE->node->data.stat == '{'
                           )
                        {
                           // tree_destruct(WORKING_TAPE->node);

                            NEXT_TAPE;

                            //printf("%s", l_name);

                            Node* equal_daddy       = nullptr,
                                * equal_left_son    = nullptr,
                                * equal_right_son   = nullptr;

                            char buffer[17];

                            size_t word_index = 0;

                            while(l_name[word_index] != '_')
                            {
                                ++word_index;
                            }

                            char* l_name_without_postfix = (char*) calloc(sizeof(char), word_index + 2);
                            assert (l_name_without_postfix);

                            strncpy(l_name_without_postfix, l_name, word_index + 1);

                            l_name_without_postfix[word_index] = '\0';

                            l_name = l_name_without_postfix;

                            size_t l_length = strlen (l_name),
                                   b_length = 0;


                            //printf("llll_____%s______llll", l_name);

                            for (size_t l_ind = 0; l_ind < *list_length; l_ind++)
                            {
                                left_son = new_node(EMPTY_NODE, EQUAL, equal_daddy, left_son);

                                if (!(equal_right_son = getE(func_param)))
                                {
                                    equal_right_son = new_node(INT);

                                    equal_right_son->data.i_num = 0;

                                    // or another type
                                }

                                equal_left_son = new_node(VARIABLE);

                                my_itoa(l_ind, buffer, 10);

                                b_length = strlen (buffer);

                                equal_left_son->cell = (char*) realloc(equal_left_son->cell, sizeof (char) * (l_length + b_length + 2));

                                //assert (l_name);

                                strcpy(equal_left_son->cell, l_name);

                                equal_left_son->cell[l_length] = '_';

                                strncpy(&equal_left_son->cell[l_length + 1], buffer, b_length);

                                equal_left_son->cell[b_length + l_length + 1] = '\0';

                                capsule_fusioning(&equal_left_son->cell,
                                                    strlen(equal_left_son->cell), func_param);

                                //
                                H_list_insert(tree, 0, equal_left_son->cell, V_VARIABLE);
                                //

                                equal_daddy = new_node(OPERATOR, EQUAL);

                                daddy_and_sons_connection(equal_daddy, equal_right_son, equal_left_son);

                                left_son->right_son = equal_daddy;

                                if (l_ind == *list_length - 1)
                                    continue;

                                if (WORKING_TAPE->node->type == OPERATOR
                                    &&
                                    WORKING_TAPE->node->data.stat == ','
                                   )
                                {
                                    //tree_destruct(WORKING_TAPE->node);

                                    NEXT_TAPE;
                                }
                               /* else
                                {

                                }*/
                            }

                            if (WORKING_TAPE->node->type == OPERATOR
                                &&
                                WORKING_TAPE->node->data.stat == '}'
                               )
                            {
                                //tree_destruct(WORKING_TAPE->node);

                                NEXT_TAPE;
                            }
                            else
                            {
                                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                                        FAILED_DATA, OPERATOR, FCB);
                            }

                            free(l_name_without_postfix);
                        }
                        else
                        {
                            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                                    FAILED_DATA, OPERATOR, FOB);
                        }
                    }
                    else
                    {
                        syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                                FAILED_DATA, OPERATOR, EQUAL);
                    }
                }
                else
                {
                    syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                            FAILED_DATA, OPERATOR, CB);
                }
            }
            else
            {
                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                        FAILED_ANOTHER);
            }
        }
        else
        {
            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                            FAILED_DATA, OPERATOR, OB);
        }
    }

    daddy_and_sons_connection(daddy, nullptr, left_son);

    return daddy;
}

Node* getIF(const FuncParameters* func_param)
{
    Node* if_root = nullptr;

    DEBUG

    if (WORKING_TAPE->node->type == FUNCTION
        &&
        WORKING_TAPE->node->data.stat == FUNC_if
       )
    {
        if_root = WORKING_TAPE->node;

        //daddy_and_sons_connection(if_root, new_node(FUNCTION, FUNC_then));

        NEXT_TAPE;

        if (WORKING_TAPE->node->type == OPERATOR
            &&
            WORKING_TAPE->node->data.stat == '('
           )
        {
            //tree_destruct(WORKING_TAPE->node);

            NEXT_TAPE;

            if_root->left_son = getE(func_param);

            /*if (if_root->left_son = getE(func_param))
            else
                syntax_error_handler;*/

            if (WORKING_TAPE->node->type == OPERATOR
                &&
                WORKING_TAPE->node->data.stat == ')'
               )
            {
                //tree_destruct(WORKING_TAPE->node);

                NEXT_TAPE;

                if (WORKING_TAPE->node->type == FUNCTION
                    &&
                    WORKING_TAPE->node->data.stat == FUNC_true
                   )
                {
                    //daddy_and_sons_connection(if_root, WORKING_TAPE->node, if_root->left_son);

                    daddy_and_sons_connection
                    (
                        if_root,
                        new_node(EMPTY_NODE, NULL_OPER,
                                    nullptr, WORKING_TAPE->node),
                        if_root->left_son
                    );

                    NEXT_TAPE;

                    if (WORKING_TAPE->node->type == OPERATOR
                        &&
                        WORKING_TAPE->node->data.stat == '{'
                       )
                    {
                        //tree_destruct(WORKING_TAPE->node);

                        NEXT_TAPE;

                        daddy_and_sons_connection(if_root->right_son->left_son, getG(func_param));

                        if (WORKING_TAPE->node->type == OPERATOR
                            &&
                            WORKING_TAPE->node->data.stat == '}'
                           )
                        {
                            //tree_destruct(WORKING_TAPE->node);

                            NEXT_TAPE;
                        }
                        else
                            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                                    FAILED_DATA, OPERATOR, FCB);
                    }
                    else
                        syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                                FAILED_DATA, OPERATOR, FOB);
                }
                else
                    syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                            FAILED_DATA, FUNCTION, FUNC_true);

                if (WORKING_TAPE->node->type == FUNCTION
                    &&
                    WORKING_TAPE->node->data.stat == FUNC_else
                   )
                {
                    if_root->right_son->right_son = WORKING_TAPE->node;

                    NEXT_TAPE;

                    if (WORKING_TAPE->node->type == OPERATOR
                        &&
                        WORKING_TAPE->node->data.stat == '{'
                       )
                    {
                        //tree_destruct(WORKING_TAPE->node);

                        NEXT_TAPE;

                        daddy_and_sons_connection(if_root->right_son->right_son, getG(func_param));

                        if (WORKING_TAPE->node->type == OPERATOR
                            &&
                            WORKING_TAPE->node->data.stat == '}'
                           )
                        {
                            //tree_destruct(WORKING_TAPE->node);

                            NEXT_TAPE;
                        }
                        else
                            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                            FAILED_DATA, OPERATOR, FCB);
                    }
                    else
                        syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                                FAILED_DATA, OPERATOR, FOB);
                }
                else
                    syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                            FAILED_DATA, FUNCTION, FUNC_else);
            }
            else
                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                            FAILED_DATA, OPERATOR, CB);
        }
        else
            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                    FAILED_DATA, OPERATOR, OB);
    }

    return if_root;
}

Node* getWHILE(const FuncParameters* func_param)
{
    Node* while_root = nullptr;

    DEBUG

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
            //tree_destruct(WORKING_TAPE->node);

            NEXT_TAPE;

            while_root->left_son = getE(func_param);

            if (WORKING_TAPE->node->type == OPERATOR
                &&
                WORKING_TAPE->node->data.stat == ')'
               )
            {
                //tree_destruct(WORKING_TAPE->node);

                NEXT_TAPE;

                if (WORKING_TAPE->node->type == OPERATOR
                    &&
                    WORKING_TAPE->node->data.stat == '{'
                   )
                {
                    //tree_destruct(WORKING_TAPE->node);

                    NEXT_TAPE;

                    while_root->right_son = getG(func_param);

                    if (WORKING_TAPE->node->type == OPERATOR
                        &&
                        WORKING_TAPE->node->data.stat == '}'
                       )
                    {
                        //tree_destruct(WORKING_TAPE->node);

                        NEXT_TAPE;
                    }
                    else
                        syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                                FAILED_DATA, OPERATOR, FCB);
                }
                else
                    syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                            FAILED_DATA, OPERATOR, FOB);
            }
            else
                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                        FAILED_DATA, OPERATOR, CB);
        }
        else
            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                    FAILED_DATA, OPERATOR, OB);
    }

    return while_root;
}

Node* getVU(const FuncParameters* func_param)
{
    Node* left_son  = nullptr,
        * right_son = nullptr,
        * daddy     = nullptr;

    DEBUG

    char* shadow_variable_name = nullptr;

    while (WORKING_TAPE->node->type == OPERATOR
           &&
           WORKING_TAPE->node->data.stat == '@'
           ||
           (WORKING_TAPE->prev->node->type == OPERATOR
             &&
             (WORKING_TAPE->prev->node->data.stat == '='
              ||
              WORKING_TAPE->prev->node->data.stat == '['
             )
             &&
             H_search_list_by_hash(tree,
                                    shadow_variable_name =
                                        create_shadow_string(WORKING_TAPE->node->cell, func_param)
                                  )
           )
          )
    {
        DEBUG

        while (WORKING_TAPE->node->type == OPERATOR
               &&
               WORKING_TAPE->node->data.stat == '@'
              )
        {
            //tree_destruct(WORKING_TAPE->node);

            NEXT_TAPE;

            daddy = new_node(EMPTY_NODE, EQUAL, getVI(func_param), daddy);

            if (WORKING_TAPE->node->type != OPERATOR
                ||
                WORKING_TAPE->node->data.stat != ';'
               )
                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                        FAILED_DATA, OPERATOR, SEMICOLON);
            else
            {
                //tree_destruct(WORKING_TAPE->node);
            }

            NEXT_TAPE;
        }

        if (shadow_variable_name)
            while (H_search_list_by_hash(tree, shadow_variable_name)
                   &&
                   WORKING_TAPE->prev->node->type == OPERATOR
                   &&
                   (WORKING_TAPE->prev->node->data.stat == '='
                    ||
                    WORKING_TAPE->prev->node->data.stat == '['
                   )
                )
            {
                DEBUG

                daddy = new_node(EMPTY_NODE, EQUAL, getNVV(func_param), daddy);

                if (WORKING_TAPE->node->type != OPERATOR
                    ||
                    WORKING_TAPE->node->data.stat != ';'
                )
                    //assert (SYNTAX_ERROR);
                    syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                            FAILED_TYPE, OPERATOR, SEMICOLON);
                else
                {
                    //tree_destruct(WORKING_TAPE->node);
                }

                NEXT_TAPE;
            }

        free(shadow_variable_name);

        shadow_variable_name = nullptr;
    }

    return daddy;
}

Node* getVI(const FuncParameters* func_param)
{
    Node* left_son  = nullptr,
        * right_son = nullptr,
        * daddy     = nullptr;

    char* variable_name = nullptr;

    DEBUG

    if (WORKING_TAPE->node->type == VARIABLE
        &&
        (WORKING_TAPE->prev->node->type == OPERATOR
         &&
         WORKING_TAPE->prev->node->data.stat == ';'
        )
       )
    {
        char* shadow_variable_name = nullptr;

        if (H_search_list_by_hash(tree,
                                    shadow_variable_name =
                                        create_shadow_string(WORKING_TAPE->node->cell, func_param)
                                 )
           )
        {
            //printf("REDEFINITION OF VARIABLE");

            //assert (SYNTAX_ERROR);

            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                    FAILED_VAR_REDECLARATION);
        }
        else
        {
            capsule_fusioning(&WORKING_TAPE->node->cell,
                                strlen(WORKING_TAPE->node->cell), func_param);

            H_list_insert(tree, 0, WORKING_TAPE->node->cell, V_VARIABLE);
        }

        free(shadow_variable_name);

        shadow_variable_name = nullptr;

        //tree->lst->next->value.integer = 0;

        daddy = new_node(OPERATOR, EQUAL, new_node(INT), WORKING_TAPE->node);

        daddy->right_son->data.i_num = 0;

        NEXT_TAPE;

        return daddy;
    }


    do
    {   // USING FOR DECETION IN FUTURE THIS COMMA
        /*if (WORKING_TAPE->next->node->type == OPERATOR
            &&
            WORKING_TAPE->next->node->data.stat == ','
           )
            tree_destruct(WORKING_TAPE->next->node);*/

        right_son = get_recursive_equal_sign(&left_son, func_param);

        daddy = new_node(EMPTY_NODE, EQUAL, right_son, daddy);

    } while (WORKING_TAPE->node->type == OPERATOR
             &&
             WORKING_TAPE->node->data.stat == ','
             &&
             (NEXT_TAPE)
            );

    return daddy;
}

Node* get_recursive_equal_sign(Node** the_last_equal_node,
                                    const FuncParameters* func_param)
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

        capsule_fusioning(&WORKING_TAPE->node->cell,
                                strlen(WORKING_TAPE->node->cell), func_param);

        char* variable_name = WORKING_TAPE->node->cell;

        if (H_search_list_by_hash(tree, variable_name))
        {
            //printf("REDEFINITION OF VARIABLE");

            //assert (SYNTAX_ERROR);

            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                    FAILED_VAR_REDECLARATION);
        }
        else
        {
            H_list_insert(tree, 0, variable_name, V_VARIABLE);
        }

        left_son = WORKING_TAPE->node;

        NEXT_TAPE;

        daddy = WORKING_TAPE->node;

        NEXT_TAPE;

        right_son = get_recursive_equal_sign(&a_last_equal_node, func_param);

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
            //printf("_________");

            *the_last_equal_node = node_cpy(left_son);

            //node_fmt_print(stdout, *the_last_equal_node);

            //printf("_________");

            daddy_and_sons_connection(daddy, right_son, left_son);
        }

        return daddy;
    }
    else
    {
        // the last elements fall here
        // like  ... = 12, ...
        //              ^
        //              ^
        //
        // like ... , z;
        //            ^
        //            ^

        Node* ptr_node = nullptr;

        if (WORKING_TAPE->next->node->type == OPERATOR
            &&
            WORKING_TAPE->next->node->data.stat == EQUAL
              &&
              (ptr_node = getE(func_param))
           )
        {
            return ptr_node;
        }
        else if (WORKING_TAPE->next->node->type == OPERATOR
                 &&
                 WORKING_TAPE->next->node->data.stat == COMMA
                 &&
                 WORKING_TAPE->node->type == VARIABLE
                )
        {
            char* shadow_variable_name = nullptr;

            if (H_search_list_by_hash(tree,
                                        shadow_variable_name =
                                            create_shadow_string(WORKING_TAPE->node->cell, func_param)
                                     )
               )
                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                        FAILED_VAR_REDECLARATION);

            //free(shadow_variable_name);

            //shadow_variable_name = nullptr;

            //capsule_fusioning(&WORKING_TAPE->node->cell,
            //                        strlen(WORKING_TAPE->node->cell), func_param);

            free(WORKING_TAPE->node->cell);

            WORKING_TAPE->node->cell = shadow_variable_name;

            H_list_insert(tree, 0, WORKING_TAPE->node->cell, V_VARIABLE);

            ptr_node = WORKING_TAPE->node;

            NEXT_TAPE; // eat the ;

            ptr_node = new_node(OPERATOR, EQUAL,
                                new_node(INT), ptr_node);

            ptr_node->right_son->data.i_num = 0;

            return ptr_node;

        }
        else
        {
            //printf("_____");

            //node_fmt_print(stdout, WORKING_TAPE->next->node);
            //node_fmt_print(stdout, WORKING_TAPE->node);

            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                    FAILED_ANOTHER);
        }

        /*Node* daddy = WORKING_TAPE->node;
        printf("_________");
        node_fmt_print(stdout, WORKING_TAPE->node);
        printf("_________");
        NEXT_TAPE;

        return daddy;*/
    }
}

Node* getNVV(const FuncParameters* func_param)
{
    Node* left_son  = nullptr,
        * daddy     = nullptr;

    HashList* detected_variable = nullptr;

    DEBUG

    if (WORKING_TAPE->node->type == VARIABLE)
    {
        if (left_son = getLI(func_param))
        {
            node_fmt_print(stdout, left_son);

            detected_variable = (HashList*) 0x11111; //something unequal 0 for detection
        }
        else
        {
            capsule_fusioning(&WORKING_TAPE->node->cell,
                                    strlen(WORKING_TAPE->node->cell), func_param);

            //printf("%s", WORKING_TAPE->node->cell);

            detected_variable
            = H_search_list_by_hash(tree, WORKING_TAPE->node->cell);

            left_son = WORKING_TAPE->node;

            NEXT_TAPE;
        }

        if (detected_variable
            &&
            WORKING_TAPE->node->type == OPERATOR
            &&
            WORKING_TAPE->node->data.stat == '='
           )
        {
            daddy = WORKING_TAPE->node;

            NEXT_TAPE;

            daddy_and_sons_connection(daddy, getCOMP(func_param), left_son);
        }
        else
        {
            //printf("VARIABLE OR SIGN '=' DIDN'T FIND!!");

            //assert (SYNTAX_ERROR);

            if (!detected_variable)
                syntax_error_handler(WORKING_TAPE->prev, __PRETTY_FUNCTION__,
                                            FAILED_VAR_NOT_INIT);
            else
                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                            FAILED_DATA, OPERATOR, EQUAL);
        }
    }

    return daddy;
}

Node* getCOMP(const FuncParameters* func_param)
{

    Node* left_son  = nullptr,
        * right_son = nullptr,
        * daddy     = nullptr;

    daddy = getE(func_param);

    while (WORKING_TAPE->node->type == FUNCTION
           &&
           (WORKING_TAPE->node->data.stat == FUNC_more
            ||
            WORKING_TAPE->node->data.stat == FUNC_less
            ||
            WORKING_TAPE->node->data.stat == FUNC_equal
            ||
            WORKING_TAPE->node->data.stat == FUNC_leseq
            ||
            WORKING_TAPE->node->data.stat == FUNC_greq
           )
          )
    {
        left_son = daddy;

        daddy = WORKING_TAPE->node;

        NEXT_TAPE;

        right_son = getE(func_param);

        daddy_and_sons_connection(daddy, right_son, left_son);
    }

    return daddy;
}

Node* getE(const FuncParameters* func_param)
{
    Node* left_son  = nullptr,
        * right_son = nullptr,
        * daddy     = nullptr;

    daddy = getT(func_param);

    DEBUG

    while (WORKING_TAPE->node->type == OPERATOR
           &&
           (WORKING_TAPE->node->data.stat == '+' || WORKING_TAPE->node->data.stat == '-'))
    {
        left_son = daddy;

        daddy = WORKING_TAPE->node;

        NEXT_TAPE;

        right_son = getT(func_param);

        daddy_and_sons_connection(daddy, right_son, left_son);
    }

    return daddy;
}

Node* getT(const FuncParameters* func_param)
{
    Node* left_son  = nullptr,
        * right_son = nullptr,
        * daddy     = nullptr;

    daddy = getD(func_param);

    DEBUG

    while (WORKING_TAPE->node->type == OPERATOR
           &&
           (WORKING_TAPE->node->data.stat == '*' || WORKING_TAPE->node->data.stat == '/'))
    {
        left_son = daddy;

        daddy = WORKING_TAPE->node;

        NEXT_TAPE;

        right_son = getD(func_param);

        daddy_and_sons_connection(daddy, right_son, left_son);
    }

    return daddy;
}

Node* getD(const FuncParameters* func_param)
{
    Node* left_son  = nullptr,
        * right_son = nullptr,
        * daddy     = nullptr;

    DEBUG

    daddy = getP(func_param);

    while (WORKING_TAPE->node->type == OPERATOR
           &&
           WORKING_TAPE->node->data.stat == '^'
          )
    {
        left_son = daddy;

        daddy = WORKING_TAPE->node;

        NEXT_TAPE;

        right_son = getP(func_param);

        daddy_and_sons_connection(daddy, right_son, left_son);
    }

    return daddy;
}

Node* getP(const FuncParameters* func_param)
{
    Node* recognized_node = nullptr;

    DEBUG

    if (WORKING_TAPE->node->type == OPERATOR
        &&
        WORKING_TAPE->node->data.stat == '('
       )
    {
        //tree_destruct(WORKING_TAPE->node);

        NEXT_TAPE;

        recognized_node = getE(func_param);

        if (WORKING_TAPE->node->type == OPERATOR
            &&
            WORKING_TAPE->node->data.stat == ')'
           )
        {
            //tree_destruct(WORKING_TAPE->node);

            NEXT_TAPE;
        }
        else
        {
            //printf("\n SYNTAX ERROR!!");

            //assert (0);

            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                            FAILED_DATA, OPERATOR, CB);
        }
    }
    else if (WORKING_TAPE->node->type == INT)
        recognized_node = getN();

    else if (recognized_node = getLI(func_param)){}

    else if (WORKING_TAPE->node->type == VARIABLE)
        recognized_node = getV(func_param);

    else
        recognized_node = getMF();

    return recognized_node;
}

Node* getMF()
{
    Node* daddy = nullptr;

    DEBUG

    if (WORKING_TAPE->node->type == FUNCTION
        &&
        (WORKING_TAPE->node->data.stat == FUNC_sin
         ||
         WORKING_TAPE->node->data.stat == FUNC_cos
         ||
         WORKING_TAPE->node->data.stat == FUNC_tg
         ||
         WORKING_TAPE->node->data.stat == FUNC_ln
        )
       )
    {
        daddy = WORKING_TAPE->node;

        NEXT_TAPE;

        //recognized_node = getE(func_param);

        if (WORKING_TAPE->node->type == OPERATOR
            &&
            WORKING_TAPE->node->data.stat == '('
           )
        {
            NEXT_TAPE;

            daddy->right_son = getN();

            if (WORKING_TAPE->node->type == OPERATOR
                &&
                WORKING_TAPE->node->data.stat == ')'
               )
                NEXT_TAPE;
            else
                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                        FAILED_DATA, OPERATOR, CB);

        }
        else
        {
            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                    FAILED_DATA, OPERATOR, OB);
        }
    }

    return daddy;
}

Node* getLI(const FuncParameters* func_param)
{
    Node*       recognized_node   = nullptr;
    HashList*   detected_variable = nullptr;

    DEBUG

    if (WORKING_TAPE->node->type == VARIABLE
        &&
        WORKING_TAPE->prev->node->type == OPERATOR
        &&
        WORKING_TAPE->prev->node->data.stat == SOB
       )
    {
        FuncParameters shadow_postfix;

        char** list_item = &WORKING_TAPE->node->cell;

        recognized_node = WORKING_TAPE->node;

        NEXT_TAPE;
        NEXT_TAPE;

        shadow_postfix.func_length = 1 + number_of_digits(WORKING_TAPE->node->data.i_num);

        shadow_postfix.func_name = (char*) calloc(sizeof (char), shadow_postfix.func_length);

        assert (shadow_postfix.func_name);

        //((char*) shadow_postfix.func_name)[0] = '_';

        itoa(WORKING_TAPE->node->data.i_num, &(((char*) shadow_postfix.func_name)[0]),10);

        capsule_fusioning(list_item,
                            strlen(*list_item), &shadow_postfix);

        capsule_fusioning(list_item,
                            strlen(*list_item), func_param);

        detected_variable = H_search_list_by_hash(tree, *list_item);

        if (!detected_variable)
        {
            //fprintf(stdout, "THIS VARIABLE DIDN'T FOUND:\"%s\"\n", WORKING_TAPE->node->cell);

            //assert(detected_variable);

            syntax_error_handler(WORKING_TAPE->next->next, __PRETTY_FUNCTION__,
                                    FAILED_VAR_NOT_INIT);
        }

        NEXT_TAPE;

        if (WORKING_TAPE->node->type != OPERATOR
            ||
            WORKING_TAPE->node->data.stat != SCB
           )
            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                    FAILED_DATA, OPERATOR, SCB);

        NEXT_TAPE;
    }

    return recognized_node;
}



Node* getV(const FuncParameters* func_param)
{
    Node*       recognized_node   = nullptr;
    HashList*   detected_variable = nullptr;

    DEBUG

    if (WORKING_TAPE->node->type == VARIABLE)
    {
        capsule_fusioning(&WORKING_TAPE->node->cell,
                            strlen(WORKING_TAPE->node->cell), func_param);

        detected_variable = H_search_list_by_hash(tree, WORKING_TAPE->node->cell);

        if (!detected_variable)
        {
            //fprintf(stdout, "THIS VARIABLE DIDN'T FOUND:\"%s\"\n", WORKING_TAPE->node->cell);

            //assert(detected_variable);

            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                    FAILED_VAR_NOT_INIT);
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

//getFuncInit::= 'function' name_of_function '(' getFuncArguments ')' '{' G '}'
//             { 'function' name_of_function '(' getFuncArguments ')' '{' G '}'
//             }*



// H_list_insert(tree, 0, equal_left_son->cell, V_VARIABLE);
// H_search_list_by_hash(tree, WORKING_TAPE->node->cell)
//
Node* getFuncInit(void)
{
    Node* recognized_node = nullptr,
        * root_node       = nullptr;

    FuncParameters func_param;

    DEBUG

    if (WORKING_TAPE->node->type == FUNCTION
        &&
        WORKING_TAPE->node->data.stat == FUNC_function
       )
    {
        NEXT_TAPE;

        if (WORKING_TAPE->node->type == USER_FUNCTION
            &&
            !H_search_list_by_hash(tree, WORKING_TAPE->node->cell)
            &&
            WORKING_TAPE->prev->node->type == OPERATOR
            &&
            WORKING_TAPE->prev->node->data.stat == OB
           )
        {
            func_param.func_name    = WORKING_TAPE->node->cell;
            func_param.func_length  = strlen(func_param.func_name);

            H_list_insert(tree, 0, (char*) func_param.func_name, V_USER_FUNCTION);

            HashList* function_list = tree->lst->next;////// the function just inserted

            //printf("___%s___", function_list->var_name);

            root_node = WORKING_TAPE->node;

            NEXT_TAPE;
            NEXT_TAPE;

            root_node->right_son = new_node(EMPTY_NODE);
            root_node->right_son->left_son = getFuncArguments(&root_node->data.i_num,
                                                                &func_param);

            function_list->var_value = root_node->data.i_num;////// a count of arg value above zero

            if (WORKING_TAPE->node->type == OPERATOR
                &&
                WORKING_TAPE->node->data.stat == CB
               )
                NEXT_TAPE;
            else
            {
                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                        FAILED_DATA, OPERATOR, CB);
            }

            //NEXT_TAPE;

            if (WORKING_TAPE->node->type == OPERATOR
                &&
                WORKING_TAPE->node->data.stat == FOB // forming open bracket
               )
            {

                NEXT_TAPE;

                root_node->right_son->right_son = getG(&func_param);

                if (WORKING_TAPE->node->type == OPERATOR
                    &&
                    WORKING_TAPE->node->data.stat == FCB // forming close brackets
                   )
                {
                    NEXT_TAPE;

                    //prev_node = root_node;

                    while (WORKING_TAPE->node->type == FUNCTION
                           &&
                           WORKING_TAPE->node->data.stat == FUNC_function
                          )
                    {
                        NEXT_TAPE;

                        if (WORKING_TAPE->node->type == USER_FUNCTION
                            &&
                            !H_search_list_by_hash(tree, WORKING_TAPE->node->cell)
                            &&
                            WORKING_TAPE->prev->node->type == OPERATOR
                            &&
                            WORKING_TAPE->prev->node->data.stat == OB
                           )
                        {
                            func_param.func_name    = WORKING_TAPE->node->cell;
                            func_param.func_length  = strlen(func_param.func_name);

                            H_list_insert(tree, 0, (char*) func_param.func_name, V_USER_FUNCTION);

                            function_list = tree->lst->next;///////the function just inserted

                            //printf("___%s___", function_list->var_name);

                            recognized_node = WORKING_TAPE->node;

                            NEXT_TAPE;
                            NEXT_TAPE;

                            recognized_node->right_son = new_node(EMPTY_NODE);
                            recognized_node->right_son->left_son = getFuncArguments(&recognized_node->data.i_num,
                                                                                        &func_param);
                            function_list->var_value = recognized_node->data.i_num;/////

                            if (WORKING_TAPE->node->type == OPERATOR
                                &&
                                WORKING_TAPE->node->data.stat == CB
                               )
                                NEXT_TAPE;
                            else
                            {
                                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                                        FAILED_DATA, OPERATOR, CB);
                            }

                            if (WORKING_TAPE->node->type == OPERATOR
                                &&
                                WORKING_TAPE->node->data.stat == FOB // forming open bracket
                               )
                            {
                                NEXT_TAPE;

                                recognized_node->right_son->right_son = getG(&func_param);

                                if (WORKING_TAPE->node->type == OPERATOR
                                    &&
                                    WORKING_TAPE->node->data.stat == FCB // forming close brackets
                                   )
                                {
                                    recognized_node->left_son = root_node;

                                    root_node = recognized_node;

                                    NEXT_TAPE;
                                }
                            }
                            //WORKING_TAPE->node->type == USER_FUNCTION;
                        }
                        else
                        {
                            if (H_search_list_by_hash(tree, WORKING_TAPE->node->cell))
                                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                                     FAILED_FUNCTION_REDECLARATION);
                            else
                                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                                     FAILED_ANOTHER);
                        }
                    }
                }
            }
            //WORKING_TAPE->node->type == USER_FUNCTION;
        }
        else
        {
            if (H_search_list_by_hash(tree, WORKING_TAPE->node->cell))
                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                     FAILED_FUNCTION_REDECLARATION);
            else
                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                     FAILED_ANOTHER);
            //   assert(0);
        }

        if (WORKING_TAPE->node->type != END_OF_TOKENS)
        {
            //printf("11%s11", WORKING_TAPE->node->cell);

            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                    FAILED_TYPE, END_OF_TOKENS);
        }
    }
    else
    {
        syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                FAILED_NOT_SINGLE_FUNCTION);
    }

    //printf("____%s_____", function_name);

    return root_node;
} //*/

Node* getFuncArguments(int* arg_value, const FuncParameters* func_param)
{
    assert(arg_value && func_param && func_param->func_name);

    Node* root = nullptr;

     // капсула для слияния

    DEBUG

    *arg_value = 0;

    while (WORKING_TAPE->node->type == VARIABLE)
    {
        capsule_fusioning(&WORKING_TAPE->node->cell,
                            strlen(WORKING_TAPE->node->cell), func_param);

        if (H_search_list_by_hash(tree, WORKING_TAPE->node->cell))
            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                    FAILED_VAR_REDECLARATION);
        else
            H_list_insert(tree, 0, WORKING_TAPE->node->cell, V_VARIABLE);

        WORKING_TAPE->node->left_son = root;

        root = WORKING_TAPE->node;

        NEXT_TAPE;

        ++(*arg_value);

        /*if (WORKING_TAPE->node->type == OPERATOR
            &&
            WORKING_TAPE->node->data.stat == COMMA
           )
            NEXT_TAPE*/
    }

    return root;
}

char* capsule_fusioning(char** cells_name, const size_t cells_length,
                            const FuncParameters* func_param)
{
    assert(cells_name && *cells_name && func_param);

    char* fusion_capsule = (char*) realloc(*cells_name,
                                    sizeof(char) * ( cells_length + func_param->func_length + 2));

    assert(fusion_capsule);

    fusion_capsule[cells_length] = '_';

    strcpy(&fusion_capsule[cells_length + 1], func_param->func_name);

    *cells_name = fusion_capsule;   // double return for
                                    // argument and return
    return fusion_capsule;          // version
}

char* create_shadow_string(const char* cell_name, const FuncParameters* func_param)
{
    size_t shadow_str_length = strlen(cell_name);

    char* return_str = (char*) calloc(sizeof(char), shadow_str_length + 2 + func_param->func_length);

    assert(return_str);

    strcpy(return_str, cell_name);

    capsule_fusioning(&return_str,  shadow_str_length, func_param);

    return return_str;
}

Node* getFU(const FuncParameters* func_param)
{
    if (WORKING_TAPE->node->type == USER_FUNCTION)
    {
        Node* call_func_node        = WORKING_TAPE->node,
            * root_call_func_node   = nullptr;

        HashList* user_func_list = nullptr;

        if ((user_func_list = H_search_list_by_hash(tree, WORKING_TAPE->node->cell))
            &&
            WORKING_TAPE->prev->node->type == OPERATOR
            &&
            WORKING_TAPE->prev->node->data.stat == OB
           )
        {
            size_t value_inter_arg = 0;

            Node* empty_recursive_node = new_node(EMPTY_NODE);

            root_call_func_node = WORKING_TAPE->node;

            root_call_func_node->type = CALL_USER_FUNCTION;

            NEXT_TAPE;
            NEXT_TAPE;

            root_call_func_node->left_son = empty_recursive_node;

            if (empty_recursive_node->right_son = getE(func_param))
                ++value_inter_arg;

            while (WORKING_TAPE->node->type == OPERATOR
                   &&
                   WORKING_TAPE->node->data.stat == COMMA
                  )
            {
                NEXT_TAPE;

                empty_recursive_node->left_son = new_node(EMPTY_NODE);

                empty_recursive_node = empty_recursive_node->left_son;

                if (empty_recursive_node->right_son = getE(func_param))
                    ++value_inter_arg;
            }

            if (value_inter_arg != user_func_list->var_value)
            {
                //printf("%d, %s:%d", value_inter_arg, user_func_list->var_name, user_func_list->var_value);

                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                        FAILED_FUNCTION_ARG_NUMBER);
            }


            if (WORKING_TAPE->node->type == OPERATOR
                &&
                WORKING_TAPE->node->data.stat == CB
            )
                NEXT_TAPE;
            else
                syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                        FAILED_TYPE, OPERATOR, CB);

        }
        else if (!H_search_list_by_hash(tree, call_func_node->cell))
        {
            syntax_error_handler(WORKING_TAPE, __PRETTY_FUNCTION__,
                                    FAILED_FUNCTION_NOT_INIT);
        }
        else
            syntax_error_handler(WORKING_TAPE->prev, __PRETTY_FUNCTION__,
                                    FAILED_DATA, OPERATOR, OB);


        return root_call_func_node;

    }
    else
        return nullptr;

}
