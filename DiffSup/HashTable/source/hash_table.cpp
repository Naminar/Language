
#include <stdlib.h>
#include "../include/hash_table.h"

Tree* do_hash_table(const char* name_inp_file, const char* name_inf_file)
{
    if (!name_inp_file && !name_inf_file)
        return nullptr;

    Tree* tree_union = (Tree*) calloc(TABLE_SIZE, sizeof(Tree));

    all_list_init(tree_union);

    processing_data(name_inp_file, tree_union);

    get_info(tree_union, name_inf_file);

    //text_generate(fopen("newtxt.txt", "w"), tree_union, 10);

    //tree_union_destructor(tree_union);

    return tree_union;
}

void text_generate(FILE* out_txt, Tree* tree_union, size_t fras_size)
{
    size_t printed_fras = 0;

    static size_t random_key = 0;

    srand(time(nullptr));

    srand(random_key + rand());

    ++random_key;

    while (printed_fras < fras_size)
    {
        ++printed_fras;

        size_t random_tree = rand() % TABLE_SIZE;

        while (tree_union[random_tree].size == 0)
        {
            ++random_tree;

            if (random_tree >= TABLE_SIZE)
                random_tree = rand() % TABLE_SIZE;
        }

        List* first_list = search_element(&tree_union[random_tree], 1 + rand() % (tree_union[random_tree].size));

        size_t counter = 0;

        while (!first_list->sentences_begin && counter < MAX_LIST_SIZE)
        {
            ++counter;

            if (first_list->links_size == 0)
            {
                random_tree = rand() % (TABLE_SIZE);

                while (tree_union[random_tree].size == 0)
                {
                    ++random_tree;

                    if (random_tree >= TABLE_SIZE)
                        random_tree = rand() % TABLE_SIZE;
                }

                first_list = search_element(&tree_union[random_tree], 1 + rand() % (tree_union[random_tree].size));
            }
            else
                first_list = first_list->word_son[rand() % first_list->links_size];
        }

        List* end_list = first_list;

        while (!end_list->sentences_end)
        {
            if (end_list->links_size == 0)
            {
                end_list->sentences_end = true;

                break;
            }

            fprintf(out_txt, "%s ", end_list->word);

            if (end_list == (first_list = end_list->word_son[rand() % first_list->links_size]))
            {
                break;
            }

            end_list = first_list;
        }

        fprintf(out_txt, "%s \n", end_list->word);
    }
}

void tree_union_destructor(Tree* tree_union)
{
    all_list_destr(tree_union);

    free(tree_union);
}

void get_info(Tree* tree_union, const char* name_inf_file)
{
    assert (tree_union && name_inf_file);

    FILE* out_file = fopen(name_inf_file, "w");

    assert (out_file);

    for (size_t tree_ind = 0; tree_ind < TABLE_SIZE; tree_ind++)
    {
        if (!tree_union[tree_ind].size)
            continue;

        fprintf(out_file, "Table number %zu\n{\n", tree_ind);

        List* next_cell = tree_union[tree_ind].lst;

        for (size_t list_ind = 0; list_ind < tree_union[tree_ind].size; list_ind++)
        {
            next_cell = next_cell->next;

            fprintf(out_file, "  %s - %zu", next_cell->word, next_cell->frequency);

            for (size_t link_ind = 0; link_ind < next_cell->links_size; link_ind++)
            {
                fprintf(out_file, " -> %s", next_cell->word_son[link_ind]->word);
            }

            fprintf(out_file, "\n");
        }

        fprintf(out_file, "}\n\n", tree_ind);
    }

    fclose(out_file);

    printf("\nYou can see information about distribution in \"%s\".\n", name_inf_file);
}

void processing_data(const char* inp_file, Tree* tree_union)
{
    assert (tree_union && inp_file);
    assert (inp_file)              ;

    FILE* read_file = fopen(inp_file, "r");

    assert (read_file);

    char read_line[NAME_LENGTH] = {};
    char next_word[NAME_LENGTH] = {};

    while (fscanf(read_file, "%"M_SCN_LENGTH"s", read_line) != EOF)
    {
        if (fscanf(read_file, "%"M_SCN_LENGTH"s", next_word) != EOF)
        {
            add_new_word(read_line, next_word, tree_union);

            fseek(read_file, 0 - strlen(next_word), SEEK_CUR);
        }
        else
        {
            add_new_word(read_line, nullptr, tree_union);
        }
    }

    fclose(read_file);
}

void matching_links(const char* next_word, List* cell, Tree* tree_union)
{
    assert(cell && tree_union);

    if (next_word)
    {
        char read_line[NAME_LENGTH] = {};

    //SEARCH THIS ELEMENT AND ADD IT IF HE DOESN'T EXISTS
    //THEN ADD LINKS TO CELL AND THATS ALL!!!

        size_t hash = DragonHash(next_word);

        List* search_cell = tree_union[hash].lst;

        size_t key_index = 0;

        for (key_index = 0; key_index < tree_union[hash].size; key_index++)
        {
            search_cell = search_cell->next;

            if (!strcmp(next_word, search_cell->word))
            {
                break;
            }
        }

        List* linked_list = search_cell;

        if (key_index == tree_union[hash].size)
        {
            list_insert(&tree_union[hash], 0, next_word);

            linked_list = tree_union[hash].lst->next;

            --linked_list->frequency;
        }

        // SIZE CONTROLE:

        if (cell->links_size_limit == cell->links_size)
        {
            cell->links_size_limit *= 2;

            cell->word_son = (List**) realloc(cell->word_son, sizeof (List*) * cell->links_size_limit);
        }

        cell->word_son[cell->links_size] = linked_list;

        ++cell->links_size;
    }
}

void add_new_word(const char* word, const char* next_word, Tree* tree_union)
{
    assert (word && tree_union);

    size_t hash = DragonHash(word);

    List* next_cell = tree_union[hash].lst;

    size_t key_index = 0;

    for (key_index = 0; key_index < tree_union[hash].size; key_index++)
    {
        next_cell = next_cell->next;

        if (!strcmp(word, next_cell->word))
        {
            ++next_cell->frequency;

            break;
        }
    }

    if (key_index == tree_union[hash].size)
    {
        list_insert(&tree_union[hash], 0, word);

        matching_links(next_word, tree_union[hash].lst->next, tree_union);
    }
    else
        matching_links(next_word, next_cell, tree_union);
}

void all_list_init(Tree* tree_union)
{
    assert (tree_union);

    for (size_t ind = 0; ind < TABLE_SIZE;ind++)
        list_init(&tree_union[ind], MAX_LIST_SIZE);
}

void all_list_destr(Tree* tree_union)
{
    assert (tree_union);

    for (size_t ind = 0; ind < TABLE_SIZE;ind++)
        list_destructor(&tree_union[ind]);
}

size_t DragonHash(const char* word)
{
    assert (word);

    size_t hash = 0;

    size_t records_key = 0;

    size_t data_key = NULL_HASH;

    const unsigned char* word_records = (const unsigned char*) word;

    size_t length = strlen(word);

    switch (length)
    {
        case 1:
        {
            return (NULL_HASH - length + word[0]) % TABLE_SIZE;;

            break;
        }

        case 2:
        {
            data_key += data_key / (word[0] * word[1]);

            data_key = data_key & (~word[1] ^ word[0]);

            records_key += word[0] * word[1];

            break;
        }
    }

    for(size_t ind = 3; ind < length; ind++)
    {
        records_key += 2 * word_records[ind - 1] + word_records[ind - 2] % 2 + word_records[ind - 3];
    }

    switch(records_key % 4)
    {
        case 0:
        {
            records_key = records_key << 4 + 1;

            break;
        }

        case 1:
        {
            records_key = records_key << 8 + 3;

            break;
        }

        case 2:
        {
            records_key = records_key << 12 + 5;

            break;
        }

        case 3:
        {
            records_key = records_key << 16 + 7;

            break;
        }

        default:
        {
            assert (0);

            break;
        }
    }

    for(size_t ind = 1; ind < length; ind++)
    {
        data_key = data_key << 3 + 1;

        data_key = data_key ^ word[ind] + data_key % 3 + word[ind] + word[ind-1] | word[ind];
    }

    size_t secureID[3 + NAME_LENGTH] = {NULL_HASH, records_key, data_key + word[0] + word[1]};

    size_t ind = 0;

    for(ind = 3; ind < length / 2; ind++)
    {
        secureID[ind] = secureID[ind - 1] + secureID[ind - 1] % 5 + 2 * secureID[ind - 2] + secureID[ind - 3] % 13 ;
    }

    return secureID[ind - 1] % TABLE_SIZE;
}
