
#include "list.h"

void H_list_init(HashTree* tree, size_t capacity)
{
    assert(tree);

    tree->lst = (HashList*) calloc(1, sizeof(HashList));

    assert(tree->lst);

    tree->capacity = capacity;

    tree->size = 0;

    tree->lst->next = tree->lst;

    tree->lst->prev = tree->lst;

    tree->lst->var_name = (char*) calloc(1, sizeof (char));
    tree->lst->var_name[0] = '\0';
}

bool H_list_insert(HashTree* tree, size_t num, char* a_var_name)
{
    printf("initialization: '%s'\n", a_var_name);

    assert (tree && a_var_name);
    assert (num >= 0 && num <= tree->size);
    assert (tree->size < tree->capacity);

    static size_t ram_place_index = 0;

    ++tree->size;

    HashList* next_cell = tree->lst;

    for (long long int ind = 0; ind < num; ind++)
    {
         next_cell = next_cell->next;
    }

    HashList* new_cell = (HashList*) calloc(1, sizeof(HashList));

    new_cell->next      = NULL;
    new_cell->prev      = NULL;

    new_cell->var_name  = (char*) calloc(strlen(a_var_name) + 1, sizeof (char));
    strcpy(new_cell->var_name, a_var_name);

    new_cell->hash      = 0;
    new_cell->ram_place = ram_place_index;

    ++ram_place_index;

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

HashList* H_search_list_by_hash(HashTree* tree, char* checked_name)
{
    assert(tree && checked_name);

    HashList* next_cell = tree->lst;

    //printf("debug");

    //printf("%p", tree->lst);

    char* the_name = tree->lst->var_name;

    size_t size_key = 0;

    while (strcmp(the_name, checked_name) && size_key < tree->size)
    {
        next_cell = next_cell->next;

        the_name = next_cell->var_name;

        ++size_key;
    }

    if (strcmp(the_name, checked_name))
        return nullptr;

    return next_cell;
}

void H_list_destructor(HashTree* tree)
{
    assert(tree);

    if (!tree->lst)
        return;

    HashList* delete_cell = tree->lst;

    HashList* next_cell = NULL;

    for (size_t transition_num = ++tree->size; transition_num > 0; --transition_num)
    {
        next_cell = delete_cell->next;

        free(delete_cell);

        delete_cell = next_cell;
    }

    tree->lst       = NULL;
    tree->capacity  =    0;
    tree->size      =    0;
}



