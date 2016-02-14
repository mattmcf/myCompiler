# IR_Gen Readme
## Matt McFarland and Yondon Fu - Delights (CS 57 - 16W)
(For more enjoyable reading, please see https://github.com/MattRMcFarland/myCompiler/tree/master/IR_gen)

## General Overview
The file structure our our intermediate code generator is as follows:

We added a source directory to manage the increasing number of source files we created:
* `src/ast.h` and `src/ast.c` : contain functions and definitions for ast nodes. Note: we added several fields to ast_node for this part of the project.
* `src/IR_gen.h` and `src/IR_gen.c` : contain CG (code generation) and label generation functions
* `src/temp_list.h` and `src/temp_list.c` : contain the temp generation function and the temp_list structure for keeping track of temps within scopes
* `src/check_sym.h` and `src/check_sym.c` : contain the set_type function and top-down type check handling.
* `src/symtab.h` and `src/symtab.c` : contain the symbol table creation functions.
* `src/types.h` : is our new struction declaration file. We had trouble with include dependencies and so we consolidated many of our structures in one file.
* `src/toktypes.h` : legacy file for pretty printing token names.
* `src/ast_stack.h` and `src/ast_stack.c` : contains implementation of ast_node stack (for symbol tables).

Top - Level Files
* `mytest.sh` : building and testing script for intermediate code generation.
* `scan.l` : flex file.
* `parser.y` : bison parsing file. A few changes made for type checking ease.
* `Makefile` : this week's makefile. Includes several executable recipes
* `IR_gen_main.c` : generates a quad list for an executable
* `check_symbols_main.c` : type checking testing file (legacy from last week).
* `symtab_main.c` : created symbol table (legacy).
* `parser_main.c` : create parse tree from tokens (legacy).

Instructions for running tests:

`cd IR_gen`

`./mytest.sh`

## Implementation Specifics

`quad` is a structure that holds a quad operation and at most 3 quad arguments. Quad arguments can hold an int literal, temp or a label. The int literal field can be used to hold an integer constant or an offset if the quad argument is being used for an array. The label field can be used for variable ids, function ids or label ids.

We use a initialize a global quad list (dynamically resizing array) that is appended to during the CG traversal process.

### Generating temps

Temps are represented by the `temp_var` structure. `new_temp` generates a temp_var for the scope of the given AST node. We explicitly save list of temps in the symbol table for their particular scope. If you wish to see these temporary symbols, you can uncomment the printing of the symboltable in the IR_gen_main.c function. (These temps also include some preliminary memory assignment work.)

### Generating labels

Labels are represented by strings. `new_label` generates a string label using the provided AST node and a name string that is to be associated with the label.

### Operations

Quad operations are defined in `IR_gen.h`. See in line comments for descriptions of each operation.

### CG

`CG` recursively traverses the AST and uses a switch statement to handle quad generation for each case, some of which include control flow such as while loops and logical statements using `&&` and `||`.

The base cases for the traversal are when we encounter an int literal, variable identifier or a string literal. The CG function will optionally pass up quad arguments with evaluated value of nested expressions.

### gen_quad
This is the function that adds a quad to the global list of quads. Each quad includes an `op` that we will use to generate target code for the y86.

### Strings

We save strings into memory. We generate an end string label quad and a definition label quad. We use a go to quad to jump straight to the end string label because we do not want to include the string defintion in the list of quad instructions, but we still want the string in memory so the quads for the string and definition label are still included even though they will never be executed.

### Arrays

When we access elements of an array we represent the element in a quad using the name of the array and an integer offset that is equal to the index. This offset will be used with the size of the element to grab the actual value in the array. If we are using an entire array we set this offset to -1 to signal that the quad argument contains an entire array.

### Control Flow

We handle the following control flow cases: for loops, while loops, if statements, if/else statements. We generate quads to reflect the evaluation process of each of them.

### Logical Statements

We handle the following logical operations: `&&`, `||` and `!`. We generate quads to reflect the evaluation process of each of them. 

### Function Declarations

We start function declarations with a prologue quad and end them with an epilogue quad. We generate an epilogue label that will be jumped to when we encounter a return. When we find a return we grab the return node's parent function and generate a go to quad to jump to the parent function's epilogue.

### Function Calls

We grab all the arguments and generate quads for them. We generate quads for the pre call and post return operations.

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




