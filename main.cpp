
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "include/node.h"
#include "include/dump.h"

//===============================================

#define DEBUG\
    printf(__PRETTY_FUNCTION__);\
    printf(" %c, %zu\n", WORKING_TAPE[IP], IP);

size_t IP = 0;

const char* WORKING_TAPE = nullptr;//������� �����

//===============================================

/*

G Grammar
E Expression
T Term
D Degree
P Primary expression
N Number

WI words identification

V Variable
F function

G::= E'$'
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

void skip_spaces(void);

int main(void)
{
    char mass[20] = {};

    scanf("%19s", mass);

    WORKING_TAPE = mass;

    Node* root = nullptr;

    printf("%d", getG(&root));

    graph_tree_dump(root);

    tree_destruct(root);
}

int getG(Node** root)
{
    assert (root);

    IP = 0;

    *root = nullptr;

    int val = getE(root);

    DEBUG

    if (WORKING_TAPE[IP] != '\0')
    {
        printf("\n SYNTAX ERROR!!");

        assert (0);
    }

    return val;
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
        val = 11;

        ++IP;
    }

    *node = new_node(VARIABLE);

    strncpy((*node)->cell, &WORKING_TAPE[IP-1], 1);

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
