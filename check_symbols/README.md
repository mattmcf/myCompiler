# Check_Symbols Readme
## Matt McFarland and Yondon Fu - Delights (CS 57 - 16W)
(For more enjoyable reading, please see https://github.com/MattRMcFarland/myCompiler/tree/master/symbol_table)

## General Overview
The file structure our our symbol table generator is as follows:

We added a source directory to manage the increasing number of source files we created:
* `src/ast.h` and `src/ast.c` : contain functions and definitions for ast nodes. Node: we added several fields to ast_node for this part of the project.
* `src/check_sym.h` and `src/check_sym.c` : contain the set_type function and top-down type checking.
* `src/symtab.h` and `src/symtab.c` : contain the symbol table creation functions.
* `src/types.h` : is our new struction declaration file. We had trouble with include dependencies and so we consolidated most of our symboltable structures in one file.
* `src/toktypes.h` : legacy file for pretty printing token names.
* `src/ast_stack.h` and `src/ast_stack.c` : contains implementation of ast_node stack (for symbol tables).

Top - Level Files
* `mytest.sh` : building and testing script for the top-down symbol checking.
* `check_symbol_main.c` : executable for type checking testing.
* `scan.l` : flex file from last week
* `parser.y` : bison parsing file. A few changes made for type checking ease.
* `Makefile` : this week's makefile. Includes several exectuable 
* `symtab_main.c` : created symbol table (legacy from last week).
* `parser_main.c` : create parse tree from tokens (legacy).

Instructions for running tests:

`cd check_symbols`

`./mytest.sh`

Instructions for running tests and error reporting:

`cd check_symbols`

`make clean && make` (makes check_symbols executable by default)

`./check_symbols < tests/tfunc.c`

If any errors occur during the construction of the symbol table, the program will report the type for the error and exit. Errors include: 
* Failure during parsing due to syntax
* Failure to create symbol table due to duplicate symbols
* Syntax errors found during type checking

Instructions to make parser_print executable that builds ast tree and symboltable

`cd check_symbols`

`make clean && make symtab`

`./symtab < ./tests/tscope.c`

Instructions to make parser_print executable that builds ast tree and tests ast_stack

`cd check_symbols`

`make clean && make parser_print`

`./parser_print < ./tests/tscope.c`

## Implementation Specifics

We mirrored the same approach to type checking as we implemented during creation of the symbol table. The function `set_type` is a called on the root node, and it gets recrusively called on roots children. Because this traversal needs to be bottom-up (parents synthesize their types from their children after checking their children for appropriate type matches), set_type is called on root's chilren before root can check the children type and set its own type. Once `set_type()` reaches a base case (a variable, a type specifier or a constant), it can set root's type and return to the parent. (When variables are referenced, `set_type()` looks up the variable's type in the symboltable. The same occurs when functions are called for the functions return values and argument types.) Each return statement also gets pointed back to its parent function. 

### Handling Function Declaration Nodes

In order to verify a Function Declaration was correct, we needed to make sure that each return statement within the function returned the correct type. In order to do this, we fashioned another function called `find_return` that holds a target return type. This functions searches all of the children nodes of the function declaration for return statements. Once one is found, the function compares it's paylod (the intended return type) against the discovered return statement. It also increments a count of found return statements. Once all of the probes of this `find_return` function return (and return 0), the handle_function_declaration caller then examines the number of found return statements. If the function returns `void` and does not contain a return statement, then the caller manually adds a return void node to the end of the function's compound statement node (this will be helpful when we traverse the tree once more to build our intermediate representation code). If the function does not return void and there are no return statements, the `set_type()` function will report an error.

### Handling Expression and r-value nodes

For expression statements and r-value (operation) nodes, the children must have matching types. Since we are not implementing type coercion or conversion right now, we don't need to worry about the complications of type coercion. So the children type and modifier fields must match for these nodes. If a mismatch occurs, the expression / r-value node's is set to a null value and an error is reported.

## Testing Files
All test files live in the tests directory

We hand wrote our answer key files by manually going through input files and constructing what we thought should be the expected symbol table structure for a particular file. We then piped our output into a .out file which we diffed with our key file to check if the actual output matched our expected output.

### tcurly.c and tcurly_ans.txt
Sample C program with numerous nested block scopes and variable declarations within them.

### tscope.c and tscope_ans.txt
Sample C program with multiple function scopes, as well as edge cases such as blocks with a single semi-colon, empty functions without any variable declarations. We do not reflect this in our key file, but we can also test for duplicate variable declarations by uncommenting line 21 in tscope.c. The output will be that a duplicate variable has been found and the symbol table program will stop running.

Note: other test input files without key files can also be found in the tests directory.



