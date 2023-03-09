
#include "../include/tree.h"

void print_in(Node* node)
{
    if (!node)
        return;

    printf("\(");

    print_in(node->left_son);

    printf(" " CELL_FMT " ", node->cell);

    print_in(node->right_son);

    printf("\)");
}

void pro_print(Node* node)
{
    if (!node)
        return;

    if (result_priority(node, node->left_son))
    {
        printf("\(");

        pro_print(node->left_son);

        printf("\)");
    }
    else
        pro_print(node->left_son);


    node_fmt_print(stdout, node);


    if (result_priority(node, node->right_son))
    {
        printf("\(");

        pro_print(node->right_son);

        printf("\)");
    }
    else
        pro_print(node->right_son);
}

bool result_priority(Node* daddy, Node* son)
{
    size_t daddy_s  = 0,
           son_s    = 0;

    if (!son)
        return false;

    if (daddy->type == FUNCTION
        &&
        son->type == FUNCTION)
    {
        return true;
    }
    else if (daddy->type == OPERATOR
             &&
             daddy->data.stat == DEGREE
                &&
                son->type == OPERATOR
                &&
                son->data.stat == DEGREE
            )
    {
        return true;
    }


    determ_priority(daddy, &daddy_s);

    determ_priority(son, &son_s);

    if (daddy_s > son_s)
        return true;

    return false;
}

void determ_priority(Node* node, size_t* the_priority)
{
    switch (node->type)
    {
        case INT:
            if (node->data.i_num < 0)
            {
                *the_priority = 0;

                return;
            }

        case DOT:
            if (node->data.d_num < (double) 0)
            {
                *the_priority = 0;

                return;
            }

        case VARIABLE:
        {
            *the_priority = INT_DOT_VARIABLE_PRIOR;

            break;
        }

        case FUNCTION:
        {
            switch (node->data.stat)
            {
                #define newoper(...)

                #define newfun(name, less, a_priority)\
                    case FUNC_##name:                 \
                    {                               \
                        *the_priority = a_priority; \
                        break;                      \
                    }                               \


                #include "../function"

                #undef newfun
                #undef newoper

            }

            break;
        }

        case OPERATOR:
        {
            switch (node->data.stat)
            {
                #define newoper(name, less, less2, a_priority)\
                    case name:                              \
                    {                                       \
                        *the_priority = a_priority;         \
                        break;                              \
                    }

                #define newfun(...)



                #include "../function"

                #undef newfun
                #undef newoper

            }

            break;
        }
    }
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
            fprintf(out_file, " %c ", node->data.stat);

            break;
        }

        case FUNCTION:
        {
            fprintf(out_file, CELL_FMT, node->cell);

            break;
        }
    }
}

void print_post(Node* node)
{
    if (!node)
        return;

    print_post(node->left_son);

    print_post(node->right_son);

    printf(CELL_FMT "  ", node->cell);
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

size_t node_depth(Node* root)
{
    if (root)
    {
        size_t left_depth  = 0,
               right_depth = 0;

        left_depth = (root->left_son  != nullptr) ? node_depth(root->left_son)  : 0;

        right_depth = (root->right_son != nullptr) ? node_depth(root->right_son) : 0;

        return ((left_depth > right_depth) ? left_depth : right_depth) + 1;
    }

    return 0;
}
