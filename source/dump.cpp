
#include "../include/dump.h"

void graph_tree_dump(Node* node)
{
    assert(node);

    FILE* dump = fopen("dump.dot", "w");

    fprintf
    (
        dump,

        "digraph G{\n size=\"100\" "

        "node [shape = record, fontname = Arial];\n    "
    );

    write_labels(node, dump);

    fprintf(dump, "}");

    fclose(dump);

    system ("dot dump.dot -Tpdf -o dump.pdf");

    system("dump.pdf");

    //system ("dot dump.dot -Tpng -o dump.png");

    //system("dump.png");
}

void write_labels(Node* node, FILE* dump)
{
    if (!node)
		return;

    fprintf
    (
        dump,

        "elem%p[label = \" {",

        node
    );

    node_fmt_print(dump, node);

    fprintf
    (
        dump,

        "} \"];\n"
    );

    if (node->left_son)
        write_transitions(node, node->left_son, "red", dump);

    if (node->right_son)
        write_transitions(node, node->right_son, "green", dump);

    write_labels(node->left_son, dump);

    write_labels(node->right_son, dump);
}

void write_transitions(Node* daddy, Node* son, char* proof, FILE* dump)
{
    fprintf
    (
        dump,

        "edge [color = %s];\n"

        "elem%p -> elem%p;",

        proof, daddy, son
    );
}
