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

### Entering and Exiting functions

When a function is called, the caller enters a pre-call phase where the arguments are pushed onto the stack in argument order (so the first argument lives at the highest offset from the callee's framepointer). Then the `call` instruction is issued. This jumps to the subroutine. In the subroutine, the callee enters a prolog phase. In the prolog phase, the former base pointer is pushed onto the stack and then the stack pointer is set as the next base pointer. (The old base pointers lives at 0(%ebp) and the return address lives at 4(%ebp).) The callee then decrements the stack pointer to below the space reserved for all of the potential temps and locals that could live in this function at its deepest scope. 

Note: We don't have caller / callee save procedures here because we have very little register management. The contents of registers don't need to be saved because for all of our operations, we load the save from memory and then save the result. It's ok that the registers are always squashed by succeeding operations. The only time you need a register value to be saved is when returning from a function, but that is protected during the post call phase.

The callee continues through it's subroutine until it hits a return statement quad. The callee places the value to be returned into the %eax register and then jumps to the callee's epilog. In the epilog, the stack pointer is moved back to the current frame pointer. Then a `popl %ebp` restores the control link to the caller's frame pointer. Finally a `ret` instruction pops the address to return to into the program counter. The caller can looks for the return value in %eax during the postcall phase.

(Quick note about passing parameters. If an array pointer is passed, then the address of the first element of the array is pushed onto the stack in the parameter region.)

### Conditional Jumps and Conditional Moves

Whenver we encounter a comparison quad (<, <=, >, >=, ==, etc.), we set a flag that describes which type of comparison is being done. Then we use the function `comp_sub` to perform a subtraction operation and conditional move operations to set a temp with either 1 or 0 depending on what type of comparison is being performed. For example, if the quad represents the comparison a < b, we will subtract b from a. If the result is less than 0, the sign flag will be set and the `cmovl` assembler operation will move a 1 to the temp. If the result is greater than or equal to 0, the sign flag will not be set and the `cmovge` assembler operation will move a 0 to the temp.

This process is useful because whenever we encounter an IFFALSE_Q quad, we only need to use the `je` assembler operation rather than any of the other conditonal jump assembler operations such as `jl` and `jg`. We can do this because the temp that is passed to an IFFALSE_Q quad will contain a 1 or 0. We perform another subtraction operation here, this time subtracting 0 from the temp value. If the result is 0, then the zero flag is set and the `je` assembler operation is triggered causing the program to jump to the label associated with this IFFALSE_Q quad.

## Testing Files
All test files live in the tests directory

### Hello World

Run `./build_ys.sh tests/helloworld.c helloworld` to observe assembly code generation and execution for a program that prints "Hello World".

### Global, Local and Nested Local Scope

Run `./build_ys.sh tests/tnested.c tnested` to observe assembly code generation and execution for a program with nested scopes.

### Subroutine with Parameter and Return Value

Run `./build_ys.sh test/first.c first` to see a routine where main calls a function and then prints the return value.

### Simple Math

Run `./build_ys.sh tests/tmath.c tmath` to observe assembly code generation and execution for a program with simple math operations.

### Array Manipulation

Run `.build_ys.sh tests/array.c array` to observe array manipulations including indexing into local / global arrays as well as passing arrays into a function.




