
#include "../include/gram&help_functions.h"

extern HashTree* tree;
extern List* WORKING_TAPE;

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

char* create_shadow_string(const char* cell_name, const FuncParameters* func_param)
{
    size_t shadow_str_length = strlen(cell_name);

    char* return_str = (char*) calloc(sizeof(char), shadow_str_length + 2 + func_param->func_length);

    assert(return_str);

    strcpy(return_str, cell_name);

    capsule_fusioning(&return_str,  shadow_str_length, func_param);

    return return_str;
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
