
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
}

bool H_list_insert(HashTree* tree, size_t num, hash_type a_hash)
{
    assert (tree);
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
    new_cell->symbol    =    0;
    new_cell->hash      = a_hash;
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

bool H_list_delete_after(HashTree* tree, size_t num)
{
    assert(tree && num >= 0);

    assert(num < tree->size);

    --tree->size;

    HashList* next_cell = tree->lst;

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


HashList* H_search_list_by_hash(HashTree* tree, hash_type a_hash)
{
    assert(tree);

    HashList* next_cell = tree->lst;

    size_t the_hash = tree->lst->hash;

    size_t size_key = 0;

    while (the_hash != a_hash && size_key < tree->size)
    {
        next_cell = next_cell->next;

        the_hash = next_cell->hash;

        ++size_key;
    }

    if (the_hash != a_hash)
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



