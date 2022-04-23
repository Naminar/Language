
#include "../include/asm.h"
#include "../HashList/list.h"

extern HashTree* tree;

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
        }

        case EMPTY_NODE:
            return;*/

        case OPERATOR:
        {
            switch (checked_node->data.stat)
            {
                case MUL:
                {
                    father_operand_command_and_pushes(asm_file, checked_node, "MUL");

                    break;
                }

                case PLUS:
                {
                    father_operand_command_and_pushes(asm_file, checked_node, "ADD");

                    break;
                }

                case DIV:
                {
                    father_operand_command_and_pushes(asm_file, checked_node, "DIV");

                    break;
                }

                case MINUS:
                {
                    father_operand_command_and_pushes(asm_file, checked_node, "SUB");

                    break;
                }

                case EQUAL:
                {
                    HashList* found_variable = H_search_list_by_hash(tree, checked_node->left_son->cell[0]);

                    switch (checked_node->right_son->type)
                    {
                        case INT:
                        {
                            fprintf(asm_file, "PUSH %d\n", checked_node->right_son->data.i_num);

                            break;
                        }

                        case DOT:
                        {
                            fprintf(asm_file, "PUSH %f\n", checked_node->right_son->data.d_num);

                            break;
                        }

                        case VARIABLE:
                        {
                            HashList* equal_variable = H_search_list_by_hash(tree, checked_node->right_son->cell[0]);

                            fprintf(asm_file, "PUSH [%zu]\n", equal_variable->ram_place);

                            break;
                        }

                        case OPERATOR:
                        {
                            break;
                        }
                    }


                    fprintf(asm_file, "POP [%zu]\n\n", found_variable->ram_place);

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

void father_operand_command_and_pushes(FILE* asm_file, Node* father, const char *const command)
{
    /*if (father->left_son->type == INT)
        fprintf(asm_file, "PUSH %d\n", father->left_son->data.i_num);

    if (father->right_son->type == INT)
        fprintf(asm_file, "POP ex\nPUSH %d\nPUSH ex\n", father->right_son->data.i_num);*/

    //fprintf(stdout, "dbg");

    if (father->left_son->type == INT)
        fprintf(asm_file, "PUSH %d\n", father->left_son->data.i_num);
    else if (father->left_son->type == VARIABLE)
    {
        HashList* found_variable = H_search_list_by_hash(tree, father->left_son->cell[0]);

        fprintf(asm_file, "PUSH [%zu]\n", found_variable->ram_place);
    }

    if (father->right_son->type == INT)
        fprintf(asm_file, "PUSH %d\n", father->right_son->data.i_num);
    else if (father->right_son->type == VARIABLE)
    {
        HashList* found_variable = H_search_list_by_hash(tree, father->right_son->cell[0]);

        fprintf(asm_file, "PUSH [%zu]\n", found_variable->ram_place);
    }

    fprintf(asm_file, "%s\n", command);
}
