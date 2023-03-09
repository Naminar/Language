
#ifndef ASM_
#define ASM_

#include "node.h"

void do_asm_translation     (Node* root);

void asm_node_translation   (FILE* asm_file, Node* node);

void check_asm_command      (FILE* asm_file, Node* checked_node);

void father_operand_command_and_pushes(FILE* asm_file, Node* father, const char *const command);

size_t number_of_digits     (size_t n);

void pop_user_function_arg  (FILE* asm_file, Node* arg_node);

void push_user_function_arg (FILE* asm_file, Node* arg_node);

char* create_capsule_name   (const char* capsule_name, size_t capsule_length,
                                const char* tag_name, size_t tag_length, size_t tag_index);

#endif // ASM_
