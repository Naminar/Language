
#include "../include/lexer.h"

size_t WORLD_LENGTH  = 10;
size_t MAX_TOKEN_NUM = 1000; //rechek real tocken num
size_t NOT_EXIST_SYM = 0;

Tree* begin_lexering(const char *const file_name)
{
    assert (file_name);

    FILE* input_file = fopen(file_name, "r");

    assert (input_file);

    Tree* tokens_tree = (Tree*) calloc(1, sizeof (Tree));

    list_init(tokens_tree, MAX_TOKEN_NUM);

    make_token(input_file, tokens_tree);

    /*fseek(input_file, 0, SEEK_END);

    size_t count = ftell(input_file);

    char* file_buffer = (char*) calloc(count, sizeof (char));

    fseek(input_file, 0, SEEK_SET);

    fread(file_buffer, sizeof (char), count, input_file);*/

    fclose(input_file);

    return tokens_tree;
}

void make_token(FILE* input_file, Tree* token_tree)
{
    assert (input_file && token_tree);

    char input_c   = ' ';

    char* string = (char*) calloc(WORLD_LENGTH, sizeof (char));

    Node* current_node = nullptr;

    int i_value = 0;

    size_t  n_new_line        =  1,
            cursor_position   = -1,
            cursor_supplement =  0,
            cursor_str_begin  =  0,
            str_size          =  0;

    while (input_c != EOF)
    {
        //printf("BEGIN||%d\n", input_c);

        while (input_c == ' ' || input_c == '\n')
        {
            //printf("SPACE\n");

            if (input_c == '\n')
            {
                ++n_new_line;

                cursor_position = 0;
            }
            else
                ++cursor_position;

            input_c = getc(input_file);
        }

        if (input_c == EOF)
            continue;


        if (('a' <= input_c && input_c <= 'z')
            ||
            ('A' <= input_c && input_c <= 'Z')
           )
        {
            while (('a' <= input_c && input_c <= 'z')
                   ||
                   ('A' <= input_c && input_c <= 'Z')
                  )
            {
                string[str_size++] = input_c;

                input_c = getc(input_file);

                ++cursor_position;
                ++cursor_supplement;
            }

            string[str_size] = '\0';


            #define newfun(name, ...)                           \
                if (!strcmp(string, #name))                      \
                {                                                 \
                    current_node = lexering_new_node(FUNCTION, FUNC_##name);\
                                                                    \
                }else                                                \

            #define newoper(...)

            #include "../function"
            {
                // add new type of node
                
                List* prev_token = search_element(token_tree, 1);

                //node_fmt_print(stdout, prev_token->node);
                
                if (prev_token
                    && 
                    prev_token->node
                    && 
                    prev_token->node->data.stat == FUNC_function
                   )
                {
                    current_node = lexering_new_node(USER_FUNCTION);

                    size_t str_len = strlen(string);

                    if (str_len > 3)
                        current_node->cell = (char*) realloc(current_node->cell, (str_len + 1) * sizeof (char));

                    assert (current_node->cell);

                    strcpy(current_node->cell, string);

                }
                else
                {
                    current_node = lexering_new_node(VARIABLE);

                    size_t str_len = strlen(string);

                    if (str_len > 3)
                        current_node->cell = (char*) realloc(current_node->cell, (str_len + 1) * sizeof (char));

                    assert (current_node->cell);

                    strcpy(current_node->cell, string);
                }
            }

            #undef newfun
            #undef newoper

            str_size = 0;
        }
        else if ('0' <= input_c && input_c <= '9')
        {
            current_node = lexering_new_node(INT);

            while ('0' <= input_c && input_c <= '9')
            {
                i_value = i_value * 10 + input_c - '0';

                input_c = getc(input_file);

                ++cursor_position;
                ++cursor_supplement;
            }

            current_node->data.i_num = i_value;

            i_value = 0;
        }
        else
        {
            // Add the relationship of the parenthesis
            // and the name of the user's function inside the function
            
            List* prev_token = nullptr;

            if (input_c == '('
                &&
                (prev_token = search_element(token_tree, 1))
                &&
                prev_token->node
                &&
                prev_token->node->type == VARIABLE
               )
            {
                current_node = lexering_new_node(OPERATOR, OB);
                
                prev_token->node->type = USER_FUNCTION;
            }
            else 
            //new code for this mark
            switch (input_c)
            {

                #define newfun(...)

                #define newoper(name, symbol, codir,...)     \
                    case codir:                               \
                    {                                          \
                        current_node = lexering_new_node(OPERATOR, name);\
                                                                 \
                        break;                                    \
                    }


                #include "../function"

                #undef newfun
                #undef newoper

                case '#':
                {
                    ++cursor_position;

                    if ((input_c = getc(input_file)) == '#')
                    {
                        ++cursor_position;

                        while ((input_c = getc(input_file)) != '#'
                                &&
                                input_c != EOF
                              )
                        {
                            if (input_c == '\n')
                            {
                                ++n_new_line;

                                cursor_position = 0;
                            }
                            else
                                ++cursor_position;
                        }


                        if ((input_c = getc(input_file)) != '#')
                            assert (0);
                    }
                    else
                    {
                        while ((input_c = getc(input_file)) != '\n'
                                &&
                                input_c != EOF
                              );

                        ++n_new_line;

                        cursor_position = 0;
                    }

                    cursor_supplement = 0;

                    input_c = getc(input_file);

                    continue; // jump to begin of while
                }

                default:
                {
                    printf("\n__%d__\nstr%zu cur%zu", input_c, n_new_line, cursor_position);

                    assert (NOT_EXIST_SYM);
                }
            }

            ++cursor_position;
            ++cursor_supplement;

            input_c = getc(input_file);
        }

        cursor_str_begin = cursor_position -  cursor_supplement + 1;

        list_insert(token_tree, 0, current_node, cursor_str_begin, n_new_line);

        cursor_supplement = 0;

        //node_fmt_print(stdout, current_node);

        //printf(" || %d || string %zu || cursor %zu\n", current_node->type, n_new_line, cursor_str_begin);
    }
}
