
#include "../include/gram&help_functions.h"

extern HashTree* tree;
extern List* WORKING_TAPE;

//=================

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

//=================


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


