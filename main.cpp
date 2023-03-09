
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

/*#include "include/node.h"
#include "include/dump.h"
#include "include/asm.h"
#include "HashList/list.h"
#include "include/lexer.h"*/

#include "include/gram&help_functions.h"

List* WORKING_TAPE = nullptr;

HashTree* tree = (HashTree*) calloc(1, sizeof (HashTree));

int main(void)
{
    Tree* tokens_list = begin_lexering("hell.txt");

    WORKING_TAPE = tokens_list->lst->prev;

    H_list_init(tree, 30);

    Node* root = getFuncInit(); //getG();

    //graph_tree_dump(root);

    do_tree_simplify(&root);

    graph_tree_dump(root);

    do_asm_translation(root);

    H_list_destructor(tree);

    tree_destruct(root);

    /*printf("%d", tokens_list->size);

    List* lll = tokens_list->lst;

    for (int i = 0; i < tokens_list->size; i++)
    {
        node_fmt_print(stdout, lll->prev->node);

        lll = lll->prev;
    }*/

    list_destructor(tokens_list);

    free(tree);

    free(tokens_list);
}
