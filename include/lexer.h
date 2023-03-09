
#ifndef LEXER_
#define LEXER_

#include "../LexerList/list_ptr.h"

Tree* begin_lexering(const char *const file_name);
void make_token     (FILE* input_file, Tree* token_tree);

#endif // LEXER_
