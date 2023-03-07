
#include "../include/node.h"
#include "../include/dump.h"

size_t MAX_NAME_LENGTH = 4;

void daddy_and_sons_connection(Node* daddy, Node* a_right_son, Node* a_left_son)
{
    assert (daddy);

    daddy->right_son = a_right_son;

    daddy->left_son  = a_left_son;
}

Node* new_node(Type a_type, OperAndFunc a_stat, Node* right_node, Node* left_node)
{
    Node* result_node = tree_construct();

    result_node->type = a_type;

    result_node->data.stat = a_stat;

    if (a_type == FUNCTION)
    {
        size_t length = 0;

        switch (a_stat)
        {
            #define newfun(name, codir, ...)                                    \
                case FUNC_##name:                                                \
                {                                                                 \
                    result_node->cell    = (char*)                                 \
                        calloc(((length = (strlen(#name) + 1)) > MAX_NAME_LENGTH)?  \
                                length: MAX_NAME_LENGTH, sizeof (char));             \
                                                                                      \
                    strcpy(result_node->cell, #name);                                  \
                                                                                        \
                    break;                                                               \
                }

            #define newoper(...)

            #include "../function"

            #undef newfun
            #undef newoper
        }
    }
    else
    {
        result_node->cell    = (char*) calloc(4, sizeof(char));

        result_node->cell[0] = (char) a_stat;
    }

    result_node->left_son = left_node;

    result_node->right_son = right_node;

    return result_node;
}

Node* tree_construct(void)
{
    Node* root = (Node*) calloc(1, sizeof(Node));

    assert (root);

    root->left_son  =   nullptr;
    root->right_son =   nullptr;
    root->cell      =   nullptr;

    return root;
}

void tree_destruct(Node* node)
{
    if (node)
    {
        tree_destruct(node->left_son);

        tree_destruct(node->right_son);

        free(node->cell);

        free(node);
    }
}

void pro_print(Node* node)
{
    if (!node)
        return;

    pro_print(node->left_son);

    node_fmt_print(stdout, node);

    pro_print(node->right_son);
}

void node_fmt_print(FILE* out_file, Node* node)
{
    switch (node->type)
    {
        case INT:
        {
            fprintf(out_file, "%d", node->data.i_num);

            break;
        }

        case DOT:
        {
            fprintf(out_file, "%.2f", node->data.d_num);

            break;
        }

        case VARIABLE:
        {
            fprintf(out_file, CELL_FMT, node->cell);

            break;
        }

        case OPERATOR:
        {
            fprintf(out_file, "%c", node->data.stat);

            break;
        }

        case FUNCTION:
        {
            fprintf(out_file, CELL_FMT, node->cell);

            break;
        }

        case USER_FUNCTION:
        {
            //fprintf(out_file, " ");
            fprintf(out_file, CELL_FMT, node->cell);
            fprintf(out_file, ", arguments - %d", node->data.i_num);

            break;
        }

        case CALL_USER_FUNCTION:
        {
            fprintf(out_file, "call function:");
            
            fprintf(out_file, CELL_FMT, node->cell);

            break;
        }
        
        case EMPTY_NODE:
        {
            fprintf(out_file, ";");

            break;
        }
    }

    fprintf(out_file, " ");
}

void do_tree_simplify(Node** node)
{
    if (!node || !*node)
        return;

    do_tree_simplify(&(*node)->left_son);

    do_tree_simplify(&(*node)->right_son);

    *node = simple_node(*node);
}

Node* simple_node(Node* tested_node)
{
    if (!tested_node)
        return nullptr;

    Node* optimized_node = tested_node;

    if (tested_node->type == EMPTY_NODE)
    {
        if (tested_node->right_son
            &&
            !tested_node->left_son
           )
        {
            optimized_node = tested_node->right_son;

            tested_node->right_son = nullptr;

            tree_destruct(tested_node);
        }

        return optimized_node;
    }
    else if (tested_node->type != OPERATOR)
        return optimized_node;

    switch (tested_node->data.stat)
    {
        case MUL:
        {
            if (tested_node->right_son->type == INT
                &&
                tested_node->left_son->type == INT
               )
            {
                tested_node->type = INT;

                tested_node->data.i_num = tested_node->right_son->data.i_num
                                          *
                                          tested_node->left_son->data.i_num;

                free(tested_node->right_son);
                free(tested_node->left_son);

                tested_node->left_son   = nullptr;
                tested_node->right_son  = nullptr;
            }
            else if ((tested_node->right_son->type == INT
                     ||
                     tested_node->left_son->type == INT)
                        &&
                          (tested_node->right_son->type == DOT
                          ||
                          tested_node->left_son->type == DOT))
            {
                tested_node->type = DOT;

                if (tested_node->left_son->type == DOT)
                {
                    tested_node->data.d_num = tested_node->right_son->data.i_num
                                              *
                                              tested_node->left_son->data.d_num;

                }
                else
                {
                    tested_node->data.d_num = tested_node->right_son->data.d_num
                                              *
                                              tested_node->left_son->data.i_num;
                }

                free(tested_node->right_son);
                free(tested_node->left_son);

                tested_node->left_son   = nullptr;
                tested_node->right_son  = nullptr;
            }
            else if (tested_node->right_son->type == INT && tested_node->right_son->data.i_num == 1)
            {
                optimized_node = tested_node->left_son;

                free(tested_node);

                free(tested_node->right_son);

            }
            else if (tested_node->right_son->type == INT && tested_node->right_son->data.i_num == 0)
            {
                tested_node->data.i_num =   0;
                tested_node->type       = INT;
                tested_node->cell[0]    = '0';

                tree_destruct(tested_node->left_son);

                tree_destruct(tested_node->right_son);

                tested_node->left_son   = nullptr;
                tested_node->right_son  = nullptr;

            }
            else if (tested_node->left_son->type == INT && tested_node->left_son->data.i_num == 1)
            {
                optimized_node = tested_node->right_son;

                free(tested_node);

                free(tested_node->left_son);
            }
            else if (tested_node->left_son->type == INT && tested_node->left_son->data.i_num == 0)
            {
                tested_node->data.i_num =   0;
                tested_node->type       = INT;
                tested_node->cell[0]    = '0';

                tree_destruct(tested_node->left_son);

                tree_destruct(tested_node->right_son);

                tested_node->left_son   = nullptr;
                tested_node->right_son  = nullptr;
            }

            break;
        }

        case PLUS:
        {
            if (tested_node->right_son->type == INT
                &&
                tested_node->left_son->type == INT
               )
            {
                tested_node->type = INT;

                tested_node->data.i_num = tested_node->right_son->data.i_num
                                          +
                                          tested_node->left_son->data.i_num;

                free(tested_node->right_son);
                free(tested_node->left_son);

                tested_node->left_son   = nullptr;
                tested_node->right_son  = nullptr;
            }
            else if ((tested_node->right_son->type == INT
                     ||
                     tested_node->left_son->type == INT)
                        &&
                          (tested_node->right_son->type == DOT
                          ||
                          tested_node->left_son->type == DOT))
            {
                tested_node->type = DOT;

                if (tested_node->left_son->type == DOT)
                {
                    tested_node->data.d_num = tested_node->right_son->data.i_num
                                              +
                                              tested_node->left_son->data.d_num;

                }
                else
                {
                    tested_node->data.d_num = tested_node->right_son->data.d_num
                                              +
                                              tested_node->left_son->data.i_num;

                }

                free(tested_node->right_son);
                free(tested_node->left_son);

                tested_node->left_son   = nullptr;
                tested_node->right_son  = nullptr;
            }
            else if (tested_node->right_son->type == INT && tested_node->right_son->data.i_num == 0)
            {
                optimized_node = tested_node->left_son;

                free(tested_node);

                free(tested_node->right_son);
            }
            else if (tested_node->left_son->type == INT && tested_node->left_son->data.i_num == 0)
            {
                optimized_node = tested_node->right_son;

                free(tested_node);

                free(tested_node->left_son);
            }

            break;
        }

        case DIV:
        {
            if (tested_node->right_son->type == INT
                &&
                tested_node->left_son->type == INT
               )
            {
                tested_node->type = INT;

                tested_node->data.i_num = tested_node->left_son->data.i_num
                                          /
                                          tested_node->right_son->data.i_num;

                free(tested_node->right_son);
                free(tested_node->left_son);

                tested_node->left_son   = nullptr;
                tested_node->right_son  = nullptr;
            }
            else if ((tested_node->right_son->type == INT
                     ||
                     tested_node->left_son->type == INT)
                        &&
                          (tested_node->right_son->type == DOT
                          ||
                          tested_node->left_son->type == DOT))
            {
                tested_node->type = DOT;

                if (tested_node->left_son->type == DOT)
                {
                    tested_node->data.d_num = tested_node->left_son->data.d_num
                                              /
                                              tested_node->right_son->data.i_num;
                }
                else
                {
                    tested_node->data.d_num = tested_node->left_son->data.d_num
                                              /
                                              tested_node->right_son->data.i_num;
                }

                free(tested_node->right_son);
                free(tested_node->left_son);

                tested_node->left_son   = nullptr;
                tested_node->right_son  = nullptr;
            }
            else if (tested_node->right_son->type == INT && tested_node->right_son->data.i_num == 1)
            {
                optimized_node = tested_node->left_son;

                free(tested_node);

                free(tested_node->right_son);
            }

            break;
        }

        case DEGREE:
        {
            if ((tested_node->left_son->type == INT  || tested_node->left_son->type == DOT)
                &&
                (tested_node->right_son->type == INT || tested_node->right_son->type == DOT)
               )
            {
                if (tested_node->left_son->type == DOT || tested_node->right_son->type == DOT)
                {
                    tested_node->type = DOT;

                    if (tested_node->left_son->type == DOT && tested_node->right_son->type == DOT)
                    {
                        tested_node->data.d_num = pow(tested_node->left_son->data.d_num,
                                                      tested_node->right_son->data.d_num);
                    }
                    else
                    {
                        tested_node->data.d_num = (tested_node->left_son->type == DOT)?

                            pow(tested_node->left_son->data.d_num,
                                tested_node->right_son->data.i_num)

                           :pow(tested_node->left_son->data.i_num,
                                tested_node->right_son->data.d_num);
                    }
                }
                else
                {
                    tested_node->type = INT;

                    tested_node->data.i_num = (int) pow(tested_node->left_son->data.i_num,
                                                        tested_node->right_son->data.i_num);
                }

                free(tested_node->right_son);
                free(tested_node->left_son);

                tested_node->left_son   = nullptr;
                tested_node->right_son  = nullptr;

                break;
            }
            else if (tested_node->left_son->type == INT && tested_node->left_son->data.i_num == 1)
            {
                optimized_node = tested_node->left_son;

                free(tested_node);

                free(tested_node->right_son);
            }
            else if (tested_node->right_son->type == INT && tested_node->right_son->data.i_num == 1)
            {
                optimized_node = tested_node->left_son;

                free(tested_node);

                free(tested_node->right_son);
            }

            break;
        }

        case MINUS:
        {
            if (tested_node->right_son->type == INT
                &&
                tested_node->left_son->type == INT
               )
            {
                tested_node->type = INT;

                tested_node->data.i_num = tested_node->left_son->data.i_num
                                          -
                                          tested_node->right_son->data.i_num;

                free(tested_node->right_son);
                free(tested_node->left_son);

                tested_node->left_son   = nullptr;
                tested_node->right_son  = nullptr;
            }
            else if ((tested_node->right_son->type == INT
                     ||
                     tested_node->left_son->type == INT)
                        &&
                          (tested_node->right_son->type == DOT
                          ||
                          tested_node->left_son->type == DOT))
            {
                tested_node->type = DOT;

                if (tested_node->left_son->type == DOT)
                {
                    tested_node->data.d_num = tested_node->left_son->data.d_num
                                              -
                                              tested_node->right_son->data.i_num;
                }
                else
                {
                    tested_node->data.d_num = tested_node->left_son->data.d_num
                                              -
                                              tested_node->right_son->data.i_num;
                }

                free(tested_node->right_son);
                free(tested_node->left_son);

                tested_node->left_son   = nullptr;
                tested_node->right_son  = nullptr;
            }
            else if (tested_node->right_son->type == INT && tested_node->right_son->data.i_num == 0)
            {
                optimized_node = tested_node->left_son;

                free(tested_node);

                free(tested_node->right_son);
            }

            break;
        }
    }

    return optimized_node;
}

Node* node_cpy(Node* main_node)
{
    if (!main_node)
        return nullptr;

    Node* copy_node = (Node*) calloc(1, sizeof(Node));

    assert(copy_node);

    *copy_node = *main_node;

    copy_node->cell = (char*) calloc(strlen(main_node->cell) + 1, sizeof(char));////

    strcpy(copy_node->cell, main_node->cell);

    copy_node->left_son = node_cpy(main_node->left_son);

    copy_node->right_son = node_cpy(main_node->right_son);

    return copy_node;
}


