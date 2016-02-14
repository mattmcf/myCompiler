# Check_Symbols Readme
## Matt McFarland and Yondon Fu - Delights (CS 57 - 16W)
(For more enjoyable reading, please see https://github.com/MattRMcFarland/myCompiler/tree/master/check_symbols)

## General Overview
The file structure our our symbol table generator is as follows:

We added a source directory to manage the increasing number of source files we created:
* `src/ast.h` and `src/ast.c` : contain functions and definitions for ast nodes. Note: we added several fields to ast_node for this part of the project.
* `src/check_sym.h` and `src/check_sym.c` : contain the set_type function and top-down type check handling.
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

We mirrored the same approach to type checking as we implemented during creation of the symbol table. The function `set_type()` is a called on the root node, and it gets recrusively called on root's children. Because this traversal needs to be bottom-up (parents synthesize their types from their children after checking their children for appropriate type matches), `set_type` is called on root's children before root can check the children type and set its own type. Once `set_type()` reaches a base case (a variable, a type specifier, or a constant), it can set root's type and return to the parent. (When variables are referenced, `set_type()` looks up the variable's type in the symboltable. The same occurs when functions are called for the functions return values and argument types.) 

### Handling Function Declaration Nodes

In order to verify a Function Declaration was correct, we needed to make sure that each return statement within the function returned the correct type. In order to do this, we fashioned another function called `find_return` that holds a target return type. This function searches all of the children nodes of the function declaration for return statements. Once one is found, the function compares it's payload (the intended return type) against the discovered return statement. It also increments a count of found return statements. Once all of the probes of this `find_return` function return (and return 0, the success status), the handle_function_declaration caller then examines the number of found return statements. If the function returns `void` and does not contain a return statement, then the caller manually adds a return void node to the end of the function's compound statement node (this will be helpful when we traverse the tree once more to build our intermediate representation code). Each return statement also gets pointed back to its parent function. If the function does not return void and there are no return statements, the `set_type()` function will report an error. 

### Handling Expression and r-value nodes

For expression statements and r-value (operation) nodes, the children must have matching types. Since we are not implementing type coercion or conversion right now, we don't need to worry about the complications of type coercion. So the children type and modifier fields must match for these nodes. If a mismatch occurs, the expression / r-value node's is set to a null value and an error is reported.

## Testing Files
All test files live in the tests directory

### tops.c and tops_ans.txt
This file includes every arithmetic operation. These tests ensure we are creating temporary varibles appropriately and are able to generate correct quads for the computation, assignment, and inequality testing operations. 

### tfunc.c and tfunc_ans.txt
tfunc.c includes a file with multiple functions called with varying number of arguments. The key case in this file is the void call of `f2` without any arguments. That was a tricky case for us to handle.

### tloops.c and tloops_ans.txt
tloops.c includes all of our control flow quads. These include for and while loops and conditional if-then-else testing. This file also highlights how return statements are handled. Which is to say that they prepare the return value in the return location and then jump to the function's epilog.


### tarr.c and tarr_ans.txt
tarr.c demonstrates how quads for handling arr referencing and arr arguments should be demonstrated. The key case here is with the call of `array_f(a, a[0], b ,10)`. `a` is referenced in its array entirety and as a element. We had to hack our way to successfully handling these call options, but the quads distinguish between an array reference and an array in its entirety with the offset of `-1`.

### test_simple.c and test_simple_ans.txt
This file includes a variety of all the interesting parts mentioned before. This file also includes a print statement of a string, which shows how we include strings into the quad list (save them inline in the quad list).




