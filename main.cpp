
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#define DEBUG\
    printf(__PRETTY_FUNCTION__);\
    printf(" %c, %zu\n", WORKING_TAPE[IP], IP);

size_t IP = 0;

const char* WORKING_TAPE = nullptr;//рабочая лента

/*

G Grammar
E Expression
T Term
D Degree
P Primary expression
N Number
V Variable

G::= E'$'
E::= T{ [+-]T }*
T::= D{ [*\]D }*
D::= P{ [^]P }*
P::= '('E')' | N | V
V::= [a-z,A-Z]
N::= [0-9]+


*/

//===================================================

int getG(void);
int getE(void);
int getT(void);
int getD(void);
int getP(void);
int getN(void);
int getV(void);
void gap_up(void);

int main(void)
{
    char mass[20] = {};

    scanf("%19s", mass);

    WORKING_TAPE = mass;

    printf("%d", getG());
}

int getG(void)
{
    IP = 0;

    int val = getE();

    DEBUG

    if (WORKING_TAPE[IP] != '$')
    {
        printf("\n SYNTAX ERROR!!");

        assert (0);
    }

    return val;
}

int getE(void)
{
    int val = getT();

    DEBUG

    while (WORKING_TAPE[IP] == '+' || WORKING_TAPE[IP] == '-')
    {
        if (WORKING_TAPE[IP++] == '+')
        {
            val += getT();
        }
        else
            val -= getT();
    }

    return val;
}

int getT(void)
{
    int val = getD();

    DEBUG

    while (WORKING_TAPE[IP] == '*' || WORKING_TAPE[IP] == '/')
    {
        if (WORKING_TAPE[IP++] == '*')
        {
            val *= getD();
        }
        else if (WORKING_TAPE[IP-1] == '/')
        {
            val /= getD();
        }
        else
            val = (int) pow(val, getD());
    }

    return val;
}

int getD(void)
{
    int val = getP();

    DEBUG

    while (WORKING_TAPE[IP] == '^')
    {
            ++IP;

            val = (int) pow(val, getP());
    }

    return val;
}

int getP(void)
{
    int val = 0;

    DEBUG

    if (WORKING_TAPE[IP] == '(')
    {
        ++IP;

        val = getE();

        if (WORKING_TAPE[IP] != ')')
        {
            printf("\n SYNTAX ERROR!!");

            assert (0);
        }
        else
            ++IP;
    }
    else if ('0' <= WORKING_TAPE[IP] && WORKING_TAPE[IP] <= '9')
        val = getN();
    else
        val = getV();

    return val;
}

int getN(void)
{
    int val         =  0;
    size_t supIP    = IP;

    DEBUG

    while ('0' <= WORKING_TAPE[IP] && WORKING_TAPE[IP] <= '9')
    {
        val = val * 10 + WORKING_TAPE[IP] - '0';

        ++IP;
    }

    if (supIP == IP)
    {
        printf("\n SYNTAX ERROR!!");

        assert (0);
    }

    return val;
}

int getV(void)
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

    if (supIP == IP)
    {
        printf("\n SYNTAX ERROR!!");

        assert (0);
    }

    return val;
}

void gap_up()
{
    while (WORKING_TAPE[IP] == ' ' || WORKING_TAPE[IP] == '\n')
        IP++;
}
