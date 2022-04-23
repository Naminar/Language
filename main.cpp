
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "include/node.h"
#include "include/dump.h"
#include "include/asm.h"
#include "HashList/list.h"

//===============================================

#define DEBUG\
    printf(__PRETTY_FUNCTION__);\
    printf(" %c, %zu\n", WORKING_TAPE[IP], IP);

size_t IP = 0;

const char* WORKING_TAPE = nullptr;//������� �����

//===============================================

const size_t SYNTAX_ERROR = 0;
/*

G Grammar
E Expression
T Term
D Degree
P Primary expression
V Variable
N Number

WI words identification(preliminary version)
F function
NVV new value of variables

G::= { '@'WI';' }* { NVV';' }* E'\0'


WI::= [a-z,A-Z]'='E{ ','[a-z,A-Z]'='E }
NVV::= [a-z,A-Z]'='E

E::= T{ [+-]T }*
T::= D{ [*\]D }*
D::= P{ [^]P }*
P::= '('E')' | N | V
V::= [a-z,A-Z]
N::= [0-9]+


tree builder:

tree model such as in differentiator

*/

//===================================================

int getG(Node** root);
int getE(Node** node);
int getT(Node** node);
int getD(Node** node);
int getP(Node** node);
int getN(Node** node);
int getV(Node** node);

void getWI(Node** root);
Node* getNVV(HashList* found_list);
void skip_spaces(void);


Node* recursive_equal_sign(size_t supIP, VarValue* value, Node** root);

HashTree* tree = (HashTree*) calloc(1, sizeof (HashTree));

int main(void)
{
    char mass[30] = {};

    scanf("%29s", mass);

    WORKING_TAPE = mass;

    Node* root = nullptr;

    printf("%d", getG(&root));

    graph_tree_dump(root);

    do_asm_translation(root);

    do_tree_simplify(&root);

    graph_tree_dump(root);

    H_list_destructor(tree);

    free(tree);

    tree_destruct(root);
}

int getG(Node** root)
{
    assert (root);

    IP = 0;

    *root = nullptr;

    H_list_init(tree, 5);

    //Node* init_variable_tree = new_node(EMPTY_NODE);

    Node* left_son = nullptr;
    Node* right_son = nullptr;

    if (WORKING_TAPE[IP] == '@')
    {
        ++IP;

        getWI(&left_son);

        if (WORKING_TAPE[IP] != ';')
            assert (0);

        ++IP;
    } // it's not necessary, but make tree beautiful

    Node* current_top = left_son;

    while (WORKING_TAPE[IP] == '@')
    {
        ++IP;

        current_top = new_node(EMPTY_NODE, EQUAL, nullptr, current_top);

        getWI(&current_top->right_son);

        if (WORKING_TAPE[IP] != ';')
            assert (0);

        ++IP;
    }

    HashList* search_list = nullptr;

    while (WORKING_TAPE[IP+1] == '=' && (search_list = H_search_list_by_hash(tree,WORKING_TAPE[IP])))
    {
        IP += 2;

        current_top = new_node(EMPTY_NODE, EQUAL, getNVV(search_list), current_top);

        if (WORKING_TAPE[IP] != ';')
            assert (0);

        ++IP;
    }

    left_son = current_top;

    int val = getE(&right_son);

    DEBUG

    if (WORKING_TAPE[IP] != '\0')
    {
        printf("\n SYNTAX ERROR!!");

        assert (0);
    }

    (*root) = new_node(EMPTY_NODE, EQUAL, right_son, left_son);

    /*H_list_destructor(tree);

    free(tree);*/

    return val;
}

void getWI(Node** root)
{
    size_t supIP    = IP;

    Node* root_left_son = nullptr;
    Node* root_right_son = nullptr;

    DEBUG

    do
    {
        if ((('a' <= WORKING_TAPE[IP] && WORKING_TAPE[IP] <= 'z')
            ||
            ('A' <= WORKING_TAPE[IP] && WORKING_TAPE[IP] <= 'Z'))
           )
        {
            ++IP;
        }

        if (WORKING_TAPE[IP] == '=')
        {
            const char variable_name = WORKING_TAPE[IP-1];

            ++IP;

            /*new_node(EMPTY_NODE, EQUAL, ,*root);

            Node* variable_init_tree = nullptr;


            new_node(OPERATOR, EQUAL, , new_node(VARIABLE))*/

            if (H_search_list_by_hash(tree, variable_name))
            {
                fprintf(stdout, "SYNTAX ERROR: REDEFINITION OF VARIABLE - '%C'\n", variable_name);

                assert (SYNTAX_ERROR);
            }

            VarValue var_value;

            //Node* latest_var_tree = nullptr;

            Node* checking_tree = recursive_equal_sign(IP, &var_value, &root_left_son);


            if (checking_tree->type == EMPTY_NODE
                ||
                (checking_tree->type == OPERATOR
                  &&
                  checking_tree->data.stat == EQUAL)
               )//(checking_tree->type != EMPTY_NODE)
            {
                *root = new_node(EMPTY_NODE, EQUAL,
                            new_node(OPERATOR, EQUAL, root_left_son, new_node(VARIABLE)),
                            checking_tree);

                (*root)->right_son->left_son->cell[0] = variable_name;
            }
            else
            {
                *root = new_node(OPERATOR, EQUAL, checking_tree, new_node(VARIABLE));

                (*root)->left_son->cell[0] = variable_name;
            }




            /*if (checking_tree->type != EMPTY_NODE
                &&
                checking_tree->type == OPERATOR
                  &&
                  checking_tree->data.stat != EQUAL)
            {
                *root = new_node(OPERATOR, EQUAL, checking_tree, new_node(VARIABLE));

                (*root)->left_son->cell[0] = variable_name;
            }
            else
            {
                *root = new_node(EMPTY_NODE, EQUAL,
                            new_node(OPERATOR, EQUAL, root_left_son, new_node(VARIABLE)),
                            checking_tree);

                (*root)->right_son->left_son->cell[0] = variable_name;
            }*/

            //root_right_son = new_node(OPERATOR, EQUAL,
            //                    latest_var_tree,
            //                    new_node(VARIABLE));



            H_list_insert(tree, 0, variable_name);

            tree->lst->next->value.integer = var_value.integer;
        }
        else
            assert(0);

    } while (WORKING_TAPE[IP++] == ',');

    --IP;

    if (supIP == IP)
    {
        printf("\n SYNTAX ERROR!!");

        assert (0);
    }
}

Node* getNVV(HashList* found_list)
{
    Node* node = new_node(OPERATOR, EQUAL, nullptr, new_node(VARIABLE));

    node->left_son->cell[0] = found_list->hash;

    found_list->value.integer = getE(&node->right_son);

    //tree_destruct(node);

    return node;
}

Node* recursive_equal_sign(size_t supIP, VarValue* value, Node** root)
{

    DEBUG

    if (WORKING_TAPE[supIP+1] == '='
        &&
          (('a' <= WORKING_TAPE[supIP] && WORKING_TAPE[supIP] <= 'z')
          ||
          ('A' <= WORKING_TAPE[supIP] && WORKING_TAPE[supIP] <= 'Z'))
       )
    {
        IP += 2;

        Node* root_left_son = nullptr;
        Node* root_right_son = nullptr;
        Node* main_express =  nullptr;

        VarValue var_value;

        const char variable_name = WORKING_TAPE[supIP];

        Node* checking_tree = recursive_equal_sign(IP, &var_value, &root_left_son);

        if (checking_tree->type == EMPTY_NODE
            ||
            (checking_tree->type == OPERATOR
              &&
              checking_tree->data.stat == EQUAL)
           )//(checking_tree->type != EMPTY_NODE)
        {
            *root = new_node(EMPTY_NODE, EQUAL,
                        new_node(OPERATOR, EQUAL, root_left_son, new_node(VARIABLE)),
                        checking_tree);

            (*root)->right_son->left_son->cell[0] = variable_name;

            main_express = *root;

            *root = node_cpy(root_left_son);

            //new_node(VARIABLE);

            // (*root)->cell[0] = root_left_son->cell[0];
        }
        else
        {
            *root = new_node(OPERATOR, EQUAL, checking_tree, new_node(VARIABLE));

            (*root)->left_son->cell[0] = variable_name;

            main_express = *root;

            *root = new_node(VARIABLE);

            (*root)->cell[0] = variable_name;
        }

        H_list_insert(tree, 0, variable_name);

        tree->lst->next->value.integer = var_value.integer;

        (*value).integer = var_value.integer;

        return main_express;
    }
    else
    {
        IP = supIP;

        Node* value_root = nullptr;

        (*value).integer = getE(&value_root);//getN(&left_son);

        (*root) = value_root;

        //tree_destruct(value_root);

        return (*root);
    }

}

int getE(Node** node)
{
    Node* left_son  = nullptr,
        * right_son = nullptr;

    int val = getT(&left_son);

    *node = left_son;

    DEBUG

    while (WORKING_TAPE[IP] == '+' || WORKING_TAPE[IP] == '-')
    {
        if (WORKING_TAPE[IP++] == '+')
        {
            val += getT(&right_son);

            *node = new_node(OPERATOR, PLUS, right_son, left_son);
        }
        else
        {
            val -= getT(&right_son);

            *node =  new_node(OPERATOR, MINUS, right_son, left_son);
        }

        left_son = *node;
    }

    return val;
}

int getT(Node** node)
{
    Node* left_son  = nullptr,
        * right_son = nullptr;

    int val = getD(&left_son);

    *node = left_son;

    DEBUG

    while (WORKING_TAPE[IP] == '*' || WORKING_TAPE[IP] == '/')
    {
        if (WORKING_TAPE[IP++] == '*')
        {
            val *= getD(&right_son);

            *node = new_node(OPERATOR, MUL, right_son, left_son);
        }
        else if (WORKING_TAPE[IP-1] == '/')
        {
            val /= getD(&right_son);

            *node = new_node(OPERATOR, DIV, right_son, left_son);
        }

        left_son = *node;
    }

    return val;
}

int getD(Node** node)
{
    Node* left_son  = nullptr,
        * right_son = nullptr;

    int val = getP(&left_son);

    *node = left_son;

    DEBUG

    while (WORKING_TAPE[IP] == '^')
    {
            ++IP;

            val = (int) pow(val, getP(&right_son));

        *node = new_node(OPERATOR, DEGREE, right_son, left_son);

        left_son = *node;
    }

    return val;
}

int getP(Node** node)
{
    int val = 0;

    DEBUG

    if (WORKING_TAPE[IP] == '(')
    {
        ++IP;

        val = getE(node);

        if (WORKING_TAPE[IP] != ')')
        {
            printf("\n SYNTAX ERROR!!");

            assert (0);
        }
        else
            ++IP;
    }
    else if ('0' <= WORKING_TAPE[IP] && WORKING_TAPE[IP] <= '9')
        val = getN(node);
    else
        val = getV(node);

    return val;
}

int getN(Node** node)
{
    int val         =  0;
    size_t supIP    = IP;

    DEBUG

    while ('0' <= WORKING_TAPE[IP] && WORKING_TAPE[IP] <= '9')
    {
        val = val * 10 + WORKING_TAPE[IP] - '0';

        ++IP;
    }

    *node = new_node(INT);

    (*node)->data.i_num = val;

    if (supIP == IP)
    {
        printf("\n SYNTAX ERROR!!");

        assert (0);
    }

    return val;
}

int getV(Node** node)
{
    int val         =  0;
    size_t supIP    = IP;

    DEBUG

    if (('a' <= WORKING_TAPE[IP] && WORKING_TAPE[IP] <= 'z')
        ||
        ('A' <= WORKING_TAPE[IP] && WORKING_TAPE[IP] <= 'Z')
       )
    {
        //*node = new_node(INT);

        *node = new_node(VARIABLE);

        (*node)->cell[0] = WORKING_TAPE[IP];

        HashList* detected_variable = H_search_list_by_hash(tree, WORKING_TAPE[IP]);


        if (!detected_variable)
        {
            fprintf(stdout, "THIS VARIABLE DIDN'T FOUND:'%c'\n", WORKING_TAPE[IP]);

            assert(detected_variable);
        }

        val  = detected_variable->value.integer;

        /*(*node)->data.i_num = val;*/

        ++IP;
    }

    if (supIP == IP)
    {
        printf("\n SYNTAX ERROR!!");

        assert (0);
    }

    return val;
}

void skip_spaces()
{
    while (WORKING_TAPE[IP] == ' ' || WORKING_TAPE[IP] == '\n')
        IP++;
}
