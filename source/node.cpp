Node* new_node(Type a_type, OperAndFunc a_stat, Node* right_node, Node* left_node)
{
    Node* result_node = tree_construct();

    result_node->cell = (char*) calloc(4, sizeof(char));

    result_node->cell[0] = (char) a_stat;

    result_node->type = a_type;

    result_node->data.stat = a_stat;

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
