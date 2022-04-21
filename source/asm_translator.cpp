
#include "../include/asm.h"

void do_asm_translation(Node* root)
{
    assert (root);

    FILE* asm_file = fopen("asm.txt", "w");

    assert (asm_file);

    asm_node_translation(asm_file, root);

    fprintf(asm_file, "OUT\nHLT");

    fclose(asm_file);
}

void asm_node_translation(FILE* asm_file, Node* node)
{
    if (!node || !asm_file)
        return;

    asm_node_translation(asm_file, node->left_son);

    asm_node_translation(asm_file, node->right_son);

    check_asm_command(asm_file, node);
}

void check_asm_command(FILE* asm_file, Node* checked_node)
{
    switch (checked_node->type)
    {
        /*case INT:
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
        }*/

        case OPERATOR:
        {
            switch (checked_node->data.stat)
            {
                case MUL:
                {
                    father_command_and_pushes(asm_file, checked_node, "MUL");

                    break;
                }

                case PLUS:
                {
                    father_command_and_pushes(asm_file, checked_node, "ADD");

                    break;
                }

                case DIV:
                {
                    father_command_and_pushes(asm_file, checked_node, "DIV");

                    break;
                }

                case MINUS:
                {
                    father_command_and_pushes(asm_file, checked_node, "SUB");

                    break;
                }
            }


            break;
        }

        /*case FUNCTION:
        {
            fprintf(out_file, CELL_FMT, node->cell);

            break;
        }*/
    }
}

void father_command_and_pushes(FILE* asm_file, Node* father, const char *const command)
{
    /*if (father->left_son->type == INT)
        fprintf(asm_file, "PUSH %d\n", father->left_son->data.i_num);

    if (father->right_son->type == INT)
        fprintf(asm_file, "POP ex\nPUSH %d\nPUSH ex\n", father->right_son->data.i_num);*/

    if (father->left_son->type == INT)
        fprintf(asm_file, "PUSH %d\n", father->left_son->data.i_num);

    if (father->right_son->type == INT)
        fprintf(asm_file, "PUSH %d\n", father->right_son->data.i_num);

    fprintf(asm_file, "%s\n", command);
}
