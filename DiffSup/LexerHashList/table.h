
#ifndef TABLE_H_

#define TABLE_H_

#include "list.h"

//===============================================

const size_t H_MAX_TABLE_SIZE = 100;
const size_t H_MAX_LIST_SIZE  = 30;

//===============================================

HashTree*   H_do_hash_table(void);
void        H_all_list_init         (HashTree* tree_union);
void        H_all_list_destr        (HashTree* tree_union);
void        H_tree_union_destructor (HashTree* tree_union);

#endif // TABLE_H_
