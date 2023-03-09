
#include "list_ptr.h"

void list_init(Tree* tree, size_t capacity)
{
    assert(tree);

    tree->lst = (List*) calloc(1, sizeof(List));

    assert(tree->lst);

    tree->capacity = capacity;

    tree->size = 0;

    tree->lst->next = tree->lst;

    tree->lst->prev = tree->lst;

    tree->lst->word_son = (List**) calloc(1, sizeof (List*));

    tree->lst->word_son[0] = nullptr;

    tree->lst->links_size = 0;

    tree->lst->links_size_limit = 1;

    tree->lst->sentences_end = false;

    tree->lst->sentences_begin = false;
}

bool list_insert(Tree* tree, size_t num, type value)
{
    assert(tree && num >= 0 && num <= tree->size && tree->size < tree->capacity);

    ++tree->size;

    List* next_cell = tree->lst;

    for (long long int ind = 0; ind < num; ind++)
    {
         next_cell = next_cell->next;
    }

    List* new_cell = (List*) calloc(1, sizeof(List));

    new_cell->next          = NULL;
    new_cell->prev          = NULL;
    new_cell->frequency     =    1;
    new_cell->word_son      = (List**) calloc(1, sizeof (List*));
    new_cell->word_son[0]   = nullptr;
    new_cell->links_size    = 0;
    new_cell->links_size_limit = 1;

    new_cell->sentences_begin   = false;
    new_cell->sentences_end     = false;

    assert (strlen(value) < NAME_LENGTH);

    strcpy(new_cell->word, value);

    end_begin_detector(new_cell);

    if (!new_cell)
    {
        printf("allocation failed, free up space and try again\n");

        return false;
    }

    if (next_cell->next != tree->lst)
    {
        new_cell->next = next_cell->next;

        new_cell->prev = next_cell;

        next_cell->next->prev = new_cell;

        next_cell->next = new_cell;
    }
    else
    {
        tree->lst->prev = new_cell;

        new_cell->prev = next_cell;

        new_cell->next = tree->lst;

        next_cell->next = new_cell;
    }

    return true;
}

void end_begin_detector(List* cell)
{
    if (cell->word[0] >= 'A' && cell->word[0] <= 'Z')

        {cell->sentences_begin = true;}

    char control_s = cell->word[strlen(cell->word) - 1];

    if (control_s == '.' || control_s == '!' || control_s == '?')

        {cell->sentences_end = true;}
}

bool list_delete_after(Tree* tree, size_t num)
{
    assert(tree && num >= 0);

    assert(num < tree->size);

    --tree->size;

    List* next_cell = tree->lst;

    for (size_t ind = 0; ind < num; ind++)
    {
         next_cell = next_cell->next;
    }

    if (next_cell->next->next)
        next_cell->next->next->prev =  next_cell;

    free(next_cell->next);

    next_cell->next = next_cell->next->next;

    return true;
}


List* search_element(Tree* tree, size_t s_index)
{
    assert(tree && s_index > 0);

    List* next_cell = tree->lst;

    for (size_t key_index = 0; key_index < s_index; key_index++)
    {
        next_cell = next_cell->next;
    }

    return next_cell;
}

void list_destructor(Tree* tree)
{
    assert(tree);

    if (!tree->lst)
        return;

    List* delete_cell = tree->lst;

    List* next_cell = NULL;

    for (size_t transition_num = ++tree->size; transition_num > 0; --transition_num)
    {
        next_cell = delete_cell->next;

        free(delete_cell->word_son);

        free(delete_cell);

        delete_cell = next_cell;
    }

    tree->lst       = NULL;
    tree->capacity  =    0;
    tree->size      =    0;
}

/*void list_dump(Tree* tree)
{
    assert(tree);

    if (!tree->lst)
    {
        printf("Please initialize the list.\n");

        return;
    }

    List* next_cell = tree->lst->next;

    FILE* dump = fopen("dump.dot", "w");

    fprintf
    (
        dump,

        "digraph G{\n rankdir=\"LR\" \n size=\"100\" "

        "node [shape = record, fontname=Arial];\n     "
    );

    fprintf
    (
        dump,

        "struct1 [label =\" "
        "<f0> lst ptr                   \\n %p  |"
        "<f1> max capacity              \\n %u  |"
        "<f2> distributed cells number  \\n %u  \"];\n"

        "struct1:f0 -> elem%p;\n",

        tree->lst,      tree->capacity,
        tree->size,     tree->lst
    );

    while (next_cell != tree->lst)  //(next_cell->next->next != NULL)
    {
        fprintf
        (
            dump,

            "elem%p -> elem%p;  "
            "elem%p[label = \" {"
            "velue  \\n %d  |   "
            "next   \\n %p  |   "
            "prev   \\n %p      "
            "} \"];\n",

            next_cell,              next_cell->next,
            next_cell->next,        next_cell->next->data,
            next_cell->next->next,  next_cell->next->prev
        );

        next_cell = next_cell->next;
    }

    fprintf
    (
        dump,

        "elem%p -> elem%p;  "
        "elem%p[label = \" {"
        "velue  \\n %d  |   "
        "next   \\n %p  |   "
        "prev   \\n %p      "
        "} \"];",

        next_cell,              next_cell->next,
        next_cell->next,        next_cell->next->data,
        next_cell->next->next,  next_cell->next->prev
    );

    fprintf(dump, "}");

    fclose(dump);

    system ("dot dump.dot -Tpdf -o dump.pdf");

    system("dump.pdf");
}

//===============================================

/*int main(void)
{
    Tree der = {}; //init your password;

    size_t capacity = 5;

    list_init(&der, capacity);

    list_insert(&der, 0, 1);
    list_insert(&der, 1, 2);
    list_insert(&der, 1, 3);
    list_insert(&der, 1, 4);
    list_insert(&der, 4, 55);

    //list_delete_after(&der, 0);

    //do_list_linearization(&der, "my"); //confirm the password

    printf("$$ %p $$\n", search_element(&der, 2));

    list_dump(&der);

    list_destructor(&der);

    printf("%p", der.lst);

    return 0;
}//*/




