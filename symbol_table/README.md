# Symbol Table Readme
## Matt McFarland and Yondon Fu - Delights (CS 57 - 16W)
(For more enjoyable reading, please see https://github.com/MattRMcFarland/myCompiler/tree/master/symbol_table)

## General Overview

We built upon the skeleton code to implement the symbol table construction. We used a recursive `traverse_ast_tree` function which walks the AST parse tree created from the bison parser and add symbols in the appropriate scopes.

