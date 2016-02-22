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




