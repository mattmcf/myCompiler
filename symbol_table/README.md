# Symbol Table Readme
## Matt McFarland and Yondon Fu - Delights (CS 57 - 16W)
(For more enjoyable reading, please see https://github.com/MattRMcFarland/myCompiler/tree/master/symbol_table)

## General Overview
The file structure our our symbol table generator is as follows:

* `scan.l` is our Flex scanner file. It is unchanged from the parser segment of the assignment.
* `parser.y` is our bison file. We had to slightly correct it's behavior for compound statement reductions since the parser assignment submission.
* `parser_main.c` is an ast tree creation and printing execution source file. It builds a parse tree and was useful for testing the ast_stack functionality.
* `ast.c` and `ast.h` contain the AST node naming conventions and functions used in the parser.
* `ast_stack.h` declares structures and functions for interacting with a stack devoted to carrying `ast_node` data types.
* `ast_stack.c` implements the functions listed in `ast_stack.h`.
* `stack_test.c` contains a testing routine of the `ast_stack` structure.
* `symtab.h` contains all of the structures and function declarations used in the symbol table. The functions and basic structures given in the skeleton were expanded upon here.
* `symtab.c` implements all of the symbol table structure functions as well as ast_node handling functions such as function declaration node and variable declaration line node.
* `symtab_main.c` contains the source code for an executable that builds a parser tree and then traverses the tree to build the symbol tables. (Reads from stdin.)
* `toktypes.h` contains token enumeration. Used for names only in bison parser.

Instructions for running tests:

`cd symbol_table`

`./mytest.sh`

Instructions for running tests and error reporting:

`cd symbol_table`

`make clean && make` (makes symtab_main executable by default)

`./symtab < tests/tscope.c`

Instructions to make parser_print executable that builds ast tree and tests ast_stack

`cd symbol_table`

`make clean && make parser_print`

`./parser_print < ./tests/tscope.c`

If any errors occur during the construction of the symbol table, the program will report the duplicate symbol responsible for the error and exit. On success, the symbol table will be printed out.




We built upon the skeleton code to implement the symbol table construction. We used a recursive `traverse_ast_tree` function which walks the AST parse tree created from the bison parser and add symbols in the appropriate scopes.

