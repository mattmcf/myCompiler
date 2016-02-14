# IR_Gen Readme
## Matt McFarland and Yondon Fu - Delights (CS 57 - 16W)
(For more enjoyable reading, please see https://github.com/MattRMcFarland/myCompiler/tree/master/check_symbols)

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
* `Makefile` : this week's makefile. Includes several executable 
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

Temps are represented by the `temp_var` structure. `new_temp` generates a temp_var for the scope of the given AST node.

### Generating labels

Labels are represented by strings. `new_label` generates a string label using the provided AST node and a name string that is to be associated with the label.

### Operations

Quad operations are defined in `IR_gen.h`. See in line comments for descriptions of each operation.

### CG

`CG` recursively traverses the AST and uses a switch statement to handle quad generation for each case, some of which include control flow such as while loops and logical statements using `$$` and `||`.

The base cases for the traversal are when we encounter an int literal, variable identifier or a string literal. The CG function will optionally pass up quad arguments with evaluated value of nested expressions.

### Strings

We save strings into memory. We generate an end string label quad and a definition label quad. We use a go to quad to jump straight to the end string label because we do not want to include the string defintion in the list of quad instructions, but we still want the string in memory so the quads for the string and definition label are still included even though they will never be executed.

### Arrays

When we access elements of an array we represent the element in a quad using the name of the array and an integer offset that is equal to the index. This offset will be used with the size of the element to grab the actual value in the array. If we are using an entire array we set this offset to -1 to signal that the quad argument contains an entire array.

### Control Flow

We handle the following control flow cases: for loops, while loops, if statements, if/else statements. We generate quads to reflect the evaluation process of each of them.

### Logical Statements

We handle the following logical operations: `$$`, `||` and `!`. We generate quads to reflect the evaluation process of each of them. 

### Function Declarations

We start function declarations with a prologue quad and end them with an epilogue quad. We generate an epilogue label that will be jumped to when we encounter a return. When we find a return we grab the return node's parent function and generate a go to quad to jump to the parent function's epilogue.

### Function Calls

We grab all the arguments and generate quads for them. We generate quads for the pre call and post return operations.

## Testing Files
All test files live in the tests directory

We manually generate our answer key files by first retrieving the output for a normal AST pretty printing operation on a given input file, which serves as the base tree structure for when we are filling in fields with our expected values. We know the base tree structure is correct based on our work on generating ASTs in assignments from previous weeks. Then we fill in the appropriate fields for each node with our expected values. When testing, we piped the output for a given input file into a .out file which we diffed with our key file to check if the actual output matched our expected output.


### tfunc.c and tfunc_ans.txt
Sample C program with function calls. We can also test our program's capability to display errors by providing more arguments than needed in a function call or attempting to assign a function return value to a variable when the function's return type is void. In these cases, the program will display an error message explaining the cause and the line number that the error occurs on.

### tscope.c and tscope_ans.txt
Sample C program with multiple function scopes, as well as edge cases such as blocks with a single semi-colon, empty functions without any variable declarations. We do not reflect this in our key file, but we can also test for duplicate variable declarations by uncommenting line 21 in tscope.c. The output will be that a duplicate variable has been found and the symbol table program will stop running.

### ttypecheck.c and ttypecheck_ans.txt
Sample C program with a main function and two other functions. The function bar contains a nested function call to the function foo. We can test for error catching with undeclared variables by uncommenting line 17, which attempts to call the foo function with an undeclared variable z. We can also test for error catching with undeclared functions by uncommenting line 21, which attempts to call an undeclared function hello. We also handle edge cases that include passing arrays as parameters and accessing their contents as single variables via indexing.


