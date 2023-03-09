
#ifndef GRAM_HELP_FUNCTIONS_

#define GRAM_HELP_FUNCTIONS_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "node.h"
#include "dump.h"
#include "asm.h"
#include "../HashList/list.h"
#include "lexer.h"

//===============================================

#define DEBUG                                   \
    printf("%s ", __PRETTY_FUNCTION__);         \
    node_fmt_print(stdout, WORKING_TAPE->node); \
    printf("\n");

#define NEXT_TAPE WORKING_TAPE = WORKING_TAPE->prev

#define PREV_TAPE WORKING_TAPE = WORKING_TAPE->next

//===============================================

typedef struct FuncParameters{

    const char* func_name   = nullptr;

    size_t func_length      = 0;

} FuncParameters;

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

const size_t SYNTAX_ERROR = 0,
             SYSTEM_ERROR = 0;

//===============================================

Node* getFuncInit(void);
Node* getFuncArguments(int* arg_value, const FuncParameters* func_param);
Node* getG(const FuncParameters* func_param);
Node* getVU(const FuncParameters* func_param);
Node* getIF(const FuncParameters* func_param);
Node* getWHILE(const FuncParameters* func_param);
Node* getSHOW(const FuncParameters* func_param);
Node* getMLU(const FuncParameters* func_param);
Node* getFU(const FuncParameters* func_param);
Node* getLST(const char* l_name, int* list_length,
                const FuncParameters* func_param, int* number_of_items_list);
Node* getVI(const FuncParameters* func_param);
Node* getNVV(const FuncParameters* func_param);
Node* getCOMP(const FuncParameters* func_param);
Node* getE(const FuncParameters* func_param);
Node* getT(const FuncParameters* func_param);
Node* getD(const FuncParameters* func_param);
Node* getP(const FuncParameters* func_param);
Node* getMF();
Node* getLI(const FuncParameters* func_param);
Node* getV(const FuncParameters* func_param);
Node* getN(void);

//-----------------------------------------------

Node* getMRX(void);

//===============================================

void syntax_error_handler(List* list_of_error_node, const char* pretty_function, ErrorCode error_code,
                            Type expected_type = EMPTY_NODE, OperAndFunc expected_data = NULL_OPER);

char* create_shadow_string(const char* cell_name, const FuncParameters* func_param);

char* capsule_fusioning(char** cells_name, const size_t cells_length,
                            const FuncParameters* func_param);

Node* init_variable_node_with_index_name(Node* node, const char* name, int number,
                                            const FuncParameters* func_param);

Node* get_recursive_equal_sign(Node** the_last_equal_node, const FuncParameters* func_param);


Node* get_recursive_FUNC_as(Node** the_last_equal_node, int*,
                                const FuncParameters* func_param, int* number_of_items_list);

Node* create_list_initialization(const char* variable_name, const char* last_variable_name,
                                    int arg_length, const FuncParameters* func_param);

char* my_itoa(int num, char* buffer, int base);

#endif // GRAM_HELP_FUNCTIONS_
