
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

size_t IP = 0;

const char* WORKING_TAPE = nullptr;//рабочая лента

int getG(void);
int getE(void);
int getT(void);
int getP(void);
int getN(void);

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

int getN(void)
{
    int val         =  0;
    size_t supIP    = IP;

    printf("getN");

    printf(" %c, %zu\n", WORKING_TAPE[IP], IP);

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

int getP(void)
{
    int val = 0;

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
    else
        val = getN();

    return val;
}

int getT(void)
{
    int val = getP();

    while (WORKING_TAPE[IP] == '*' || WORKING_TAPE[IP] == '/')
    {
        if (WORKING_TAPE[IP++] == '*')
        {
            val *= getP();
        }
        else
            val /= getP();
    }

    return val;
}
