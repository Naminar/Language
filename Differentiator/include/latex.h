
#ifndef LATEX_

#define LATEX_

#include "differentiator.h"
#include "../HashTable/include/hash_table.h"

//===============================================

void do_header(FILE* tex_file, Node* root);
void do_latex_document(Node* root);
void forming_notes(FILE* tex_file, Node* node);
void latex_node_fmt(FILE* tex_file, Node* node);

//===============================================

const char *const latex_header =
"\\documentclass{article}\n"
"\\usepackage[utf8]{inputenc}\n"
"\\usepackage[english, russian]{babel}\n"
"\\usepackage{amsmath}\n"
"\\title{How to do Petrovich's labs}\n"
"\\usepackage[left=2.5cm,right=2.5cm,\n"
"    top=2cm,bottom=2cm,bindingoffset=0cm]{geometry}\n"
"\\usepackage{graphicx}\n"
"\\graphicspath{{pictures/}}\n"
"\\DeclareGraphicsExtensions{.pdf,.png,.jpg}\n"
"\\usepackage{amsfonts}\n"
"\\usepackage{amssymb}\n"
"\\usepackage{comment}\n"
"\\usepackage[unicode, pdftex]{hyperref}\n"
"\\usepackage[dvipsnames]{xcolor}\n"
"\\begin{document}\n"
"\\date{\\selectlanguage{english}\\today}\n"
"\\maketitle\n"
"\\newcommand{\\MIPT}{\\href{https://mipt.ru/}{\\textcolor{NavyBlue}{\\bold {MIPT}}}}\n";

const char *const latex_end =
"\n\\section*{\\centering Source}"
"\\subsection*{Many thanks to \\href{https://clck.ru/eow9c}{\\textcolor{purple}{Petrovich Alexander Yurievich}}"
" and \\href{https://clck.ru/eow9n}{\\textcolor{blue}{Barabanshchikov Alexander Vladimirovich.}}}"
"\\subsubsection*{\\centering Books}"
" \\newline\n"
" \\newline\n"

"Lectures on mathematical analysis. In 3 p. "
"Part 1. Introduction to Mathematical Analysis. \\newline\n"

"Lectures on mathematical analysis. In 3 p. "
"Part 2. Multidimensional analysis, integrals and series. \\newline\n"

"Lectures on mathematical analysis. In 3 p. "
"Part 3. Multiple integrals, field theory, harmonic analysis. \\newline\n"
"\n\\end{document}";

#endif // LATEX_
