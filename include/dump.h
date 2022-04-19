
#ifndef DUMP_

#define DUMP_

#include "node.h"

//===============================================

void graph_tree_dump    (Node* node);
void write_labels       (Node* node, FILE* dump);
void write_transitions  (Node* daddy, Node* son, char* proof, FILE* dump);

//===============================================

#endif // DUMP_
