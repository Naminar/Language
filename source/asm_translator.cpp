
#include "../include/asm.h"
#include "../HashList/list.h"

void pop_user_function_arg(FILE* asm_file, Node* arg_node);
void push_user_function_arg(FILE* asm_file, Node* arg_node);
char* create_capsule_name(const char* capsule_name, size_t capsule_length,
                            const char* tag_name, size_t tag_length, size_t tag_index);
size_t number_of_digits(size_t n);

extern HashTree* tree;

const char*  IF_CAPSULE      = "IF"    ,
          *  WHILE_CAPSULE   = "WHILE" ,
          *  FALSE_TAG       = "_FALSE",
          *  END_TAG         = "_END"  ,
          *  HEAD_TAG        = "_HEAD" ;

const size_t  IF_LENGTH       = 2,
              WHILE_LENGTH    = 5,
              FALSE_LENGTH    = 6,
              END_LENGTH      = 4,
              HEAD_LENGTH     = 5;

size_t TAG_INDEX = 0;

void do_asm_translation(Node* root)
{
    assert (root);

    FILE* asm_file = fopen("asm.txt", "w");

    assert (asm_file);

    //pro_print(root);

    //asm_node_translation(asm_file, root);

    check_asm_command(asm_file, root); //the main programm, on the top

    asm_node_translation(asm_file, root->right_son);

    fprintf(asm_file, "HLT\n\n");

    asm_node_translation(asm_file, root->left_son);

    fclose(asm_file);
}

void asm_node_translation(FILE* asm_file, Node* node)
{
    if (!node || !asm_file)
        return;

    //printf("  ");
    //node_fmt_print(stdout, node);
    //printf("  \n");

    if (node->type == USER_FUNCTION)
    {
        fprintf(asm_file,"\n%s:\n", node->cell);

        check_asm_command(asm_file, node);

        asm_node_translation(asm_file, node->right_son);

        fprintf(asm_file,"RET\n\n");

        asm_node_translation(asm_file, node->left_son);
    }
    else if (node->type == CALL_USER_FUNCTION)
    {
        check_asm_command(asm_file, node);
    }
    else if (node->type == FUNCTION && node->data.stat == FUNC_if)
    {       
        size_t if_index = TAG_INDEX;

        ++TAG_INDEX;

        father_operand_command_and_pushes(asm_file, node, nullptr);

        fprintf(
                asm_file, "PUSH 0\n \tJBL %s\n\n",
                create_capsule_name(IF_CAPSULE, IF_LENGTH, FALSE_TAG, FALSE_LENGTH, if_index)
                );

        asm_node_translation(asm_file, node->right_son->left_son);

        fprintf(
                asm_file, "\tJMP %s\n\t%s:\n\n",
                create_capsule_name(IF_CAPSULE, IF_LENGTH, END_TAG, END_LENGTH, if_index),
                create_capsule_name(IF_CAPSULE, IF_LENGTH, FALSE_TAG, FALSE_LENGTH, if_index)
                );

        asm_node_translation(asm_file, node->right_son->right_son);

        fprintf(
                asm_file, "\t%s:\n\n",
                create_capsule_name(IF_CAPSULE, IF_LENGTH, END_TAG, END_LENGTH, if_index)
                );
    }
    else if (node->type == FUNCTION && node->data.stat == FUNC_while)
    {       
        size_t while_index = TAG_INDEX;

        ++TAG_INDEX;

        fprintf(
                asm_file, "\t%s:\n\n",
                create_capsule_name(WHILE_CAPSULE, WHILE_LENGTH, HEAD_TAG, HEAD_LENGTH, while_index)
                );
        
        father_operand_command_and_pushes(asm_file, node, nullptr);

        fprintf(
                asm_file, "PUSH 0\n \tJBL %s\n\n",
                create_capsule_name(WHILE_CAPSULE, WHILE_LENGTH, END_TAG, END_LENGTH, while_index)
                );

        asm_node_translation(asm_file, node->right_son);

        fprintf(
                asm_file, "\tJMP %s\n\t%s:\n\n",
                create_capsule_name(WHILE_CAPSULE, WHILE_LENGTH, HEAD_TAG, HEAD_LENGTH, while_index),
                create_capsule_name(WHILE_CAPSULE, WHILE_LENGTH, END_TAG, END_LENGTH, while_index)
                );

        //asm_node_translation(asm_file, node->right_son->right_son);

        /*fprintf(
                asm_file, "\t%s:\n\n",
                create_capsule_name(IF_CAPSULE, IF_LENGTH, END_TAG, END_LENGTH, if_index)
                );*/
    }
    else
    {
        asm_node_translation(asm_file, node->left_son);

        asm_node_translation(asm_file, node->right_son);

        check_asm_command(asm_file, node);
    }
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
                    HashList* found_variable = H_search_list_by_hash(tree, checked_node->left_son->cell);

                    printf("%s; ", checked_node->left_son->cell);

                    assert(found_variable);

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
                            HashList* equal_variable = H_search_list_by_hash(tree, checked_node->right_son->cell);

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

        case USER_FUNCTION:
        {
            if (checked_node->data.i_num
                &&
                checked_node->right_son
                &&
                checked_node->right_son->left_son
               )
            {
                fprintf(asm_file,"\n");

                pop_user_function_arg(asm_file, checked_node->right_son->left_son);

                fprintf(asm_file,"\n");
            }

            break;
        }

        case CALL_USER_FUNCTION:
        {
            if (checked_node->left_son)
                push_user_function_arg(asm_file, checked_node->left_son);

            fprintf(asm_file,"CALL %s\n", checked_node->cell);

            break;
        }

        case FUNCTION:
        {
            switch (checked_node->data.stat)
            {
                case FUNC_show:
                {
                    father_operand_command_and_pushes(asm_file, checked_node, "OUT");

                    break;
                }

            }
        }
    }
}

char* create_capsule_name(const char* capsule_name, size_t capsule_length,
                            const char* tag_name, size_t tag_length, size_t tag_index)
{
    char* ret_capsule = (char*) calloc(sizeof (char), capsule_length + tag_length
                                        + number_of_digits(tag_index) + 2);

    assert (ret_capsule);

    strcpy(ret_capsule, capsule_name);

    strcpy(&ret_capsule[capsule_length], tag_name);

    ret_capsule[capsule_length + tag_length] = '_';

    itoa(tag_index, &ret_capsule[capsule_length + tag_length + 1], 10);

    return ret_capsule;
}

size_t number_of_digits(size_t n)
{
    if (n / 10 == 0)
        return 1;

    size_t number = 0;

    while (n > 0)
    {
        n /= 10;

        ++number;
    }

    return number;
}

void push_user_function_arg(FILE* asm_file, Node* arg_node)
{

    printf("____");
    node_fmt_print(stdout, arg_node);
    printf("____\n");

    if (arg_node->type == EMPTY_NODE)
    {
        push_user_function_arg(asm_file, arg_node->right_son);

        push_user_function_arg(asm_file, arg_node->left_son);
    }
    else
    {
        if (arg_node->type ==  VARIABLE)
        {
            HashList* found_variable = H_search_list_by_hash(tree, arg_node->cell);

            fprintf(asm_file, "PUSH [%zu]\n\n", found_variable->ram_place);
        }
        else
        {
            asm_node_translation(asm_file, arg_node);

            //asm_node_translation(asm_file, arg_node->right_son);
            //check_asm_command(asm_file, arg_node);
        }
    }
}

void pop_user_function_arg(FILE* asm_file, Node* arg_node)
{
    HashList* found_variable = H_search_list_by_hash(tree, arg_node->cell);

    fprintf(asm_file, "POP [%zu]\n", found_variable->ram_place);

    if (arg_node->left_son)
        pop_user_function_arg(asm_file, arg_node->left_son);
}

void father_operand_command_and_pushes(FILE* asm_file, Node* father, const char *const command)
{
    /*if (father->left_son->type == INT)
        fprintf(asm_file, "PUSH %d\n", father->left_son->data.i_num);

    if (father->right_son->type == INT)
        fprintf(asm_file, "POP ex\nPUSH %d\nPUSH ex\n", father->right_son->data.i_num);*/

    //fprintf(stdout, "dbg");
    if (father->left_son)
    {
        if (father->left_son->type == INT)
            fprintf(asm_file, "PUSH %d\n", father->left_son->data.i_num);

        else if (father->left_son->type == VARIABLE)
        {
            HashList* found_variable = H_search_list_by_hash(tree, father->left_son->cell);

            fprintf(asm_file, "PUSH [%zu]\n", found_variable->ram_place);
        }
    }

    if (father->right_son)
    {
        if (father->right_son->type == INT)
            fprintf(asm_file, "PUSH %d\n", father->right_son->data.i_num);

        else if (father->right_son->type == VARIABLE)
        {
            HashList* found_variable = H_search_list_by_hash(tree, father->right_son->cell);

            fprintf(asm_file, "PUSH [%zu]\n", found_variable->ram_place);
        }
    }

    if (command)
        fprintf(asm_file, "%s\n\n", command);
}
