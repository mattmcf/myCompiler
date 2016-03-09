 # Target Code Generation Readme
## Matt McFarland and Yondon Fu - Delights (CS 57 - 16W)
(For more enjoyable reading, please see https://github.com/MattRMcFarland/myCompiler/tree/master/Target_Code)

## General Overview
The file structure our our target code generator is as follows:

We added a source directory to manage the increasing number of source files we created:
* `src/ast.h` and `src/ast.c` : contain functions and definitions for ast nodes.
* `src/y86_code_gen.h` and `src/y86_code_gen.c` : contain target code (assembler) generation functions
* `src/IR_gen.h` and `src/IR_gen.c` : contain CG (code generation) and label generation functions
* `src/temp_list.h` and `src/temp_list.c` : contain the temp generation function and the temp_list structure for keeping track of temps within scopes
* `src/check_sym.h` and `src/check_sym.c` : contain the set_type function and top-down type check handling.
* `src/symtab.h` and `src/symtab.c` : contain the symbol table creation functions.
* `src/types.h` : is our new struction declaration file. We had trouble with include dependencies and so we consolidated many of our structures in one file.
* `src/toktypes.h` : legacy file for pretty printing token names.
* `src/ast_stack.h` and `src/ast_stack.c` : contains implementation of ast_node stack (for symbol tables).

Top - Level Files
* `build_ys.sh` : building and executing target code generator on particular test files
* `scan.l` : flex file.
* `parser.y` : bison parsing file. A few changes made for type checking ease.
* `Makefile` : this week's makefile. Includes several executable recipes
* `Target_Code_main.c` : generates target code (`.ys` file)

Instructions for running tests:

`cd Target_Code`

`./build_ys.sh tests/<input_file_name> <output_file_name>`

## Extra Features

### sizeof()

To implement the sizeof operator we added a `byte_size` field to the `var_symbol` structs contained in our `symnode` structs. Whenever we add a `symnode` to our symbol table, we call `handle_func_decl_node`, which might handle function argument variables, or `handle_var_decl_node`, which might handle variable declarations. In each function, if we find an integer variable, we set the `byte_size` to 4 and if we find an array variable, we set the `byte_size` to 4 * length of the array.

In `y86_code_gen.c`, we handle SIZEOF_Q quads by first checking if the argument for the operator is an array, an element in the array or just an integer variable. If the argument is an array or an integer variable, we just move the value of the `byte_size` field into the temp that is returned by the sizeof operation. If the argument is an element in an array, we grab the corresponding size for the type of the array element (will be 4 for an integer) and move that value into the temp returned by the sizeof operation.

### Break and Continue

To implement break and continue statements, we needed a way to jump to parent control structures from a particular break or continue node, so we added pointers from all AST nodes to their parents. We use the function `post_process_ast` in `ast.c` to recursively add parent pointers for all nodes in an AST after first generating the AST using our parser.

We also define a `lookup_parent_block` function in `ast.c` to find the AST node that represents the parent control structure for a given break or continue AST node. The function follows parent pointers up from an AST node until it reaches a parent that represents a control structure and then returns that node. If there are no parents that represent control structures, the function returns NULL.

We handle quad generation for break and continue AST nodes in `IR_gen.c`. For a break node, we find the parent control structure using `lookup_parent_block` and switch on the type of the control structure. We create an exit label based on the control structure's node ID and the type of control structure (ex. for a while loop we create a label of the format ID_WHILE_EXIT). We then generate a GOTO_Q quad that jumps to this exit label. For a continue node, we find the parent control structure using `lookup_parent_block` and switch on the type of the control structure. We create a test label (indicates the testing portion of a loop) based on the control structure's node ID and the type of control structure (ex. for a while loop we create a label of the format ID_WHILE_TEST). Note that the label for a for loop is slightly different because we need to make sure to update the for loop variable before jumping to the test portion of the loop. Consequently, we instead create a label that designates the update portion of the label of the form ID_FOR_UPDATE. We then generate a GOTO_Q quad that jumps to this label.

### Post Increment and Post Decrement

Originally our compiler only supported pre increment and pre decrement operations. In this final submission we also implemented post increment and post decrement operations. Increment and decrement operations work similarly, one increases a variable by 1 while the other decreases a variable by 1, so for the purposes of explaining pre operations and post operations we will just talk about increment.

Both pre increment and post increment increase the value of a variable. The main difference between the two operations is in the value they return. Pre increment returns the increased value of the variable. Post increment returns the original value of the variable before it is increased.

In our implementation we separate our original INC_Q quads into a PRE_INC_Q quad and a POST_INC_Q quad. In `y86_code_gen.c`, when we find a PRE_INC_Q quad we update the variable using an addition operation and also move the updated value into a temp that is returned by the operation. When we find a POST_INC_Q quad, we first move the original value of the variable into a temp that is returned by the operation and then we update the variable using an addition operation.

## Implementation Specifics

## Testing Files
All test files live in the tests directory

### Hello World

Run `./build_ys.sh tests/helloworld.c helloworld` to observe assembly code generation and execution for a program that prints "Hello World".

### Global, Local and Nested Local Scope

Run `./build_ys.sh tests/tnested.c tnested` to observe assembly code generation and execution for a program with nested scopes.

### Subroutine with Parameter and Return Value

### Simple Math

Run `./build_ys.sh tests/tmath.c tmath` to observe assembly code generation and execution for a program with simple math operations.




