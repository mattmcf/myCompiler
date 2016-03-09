 # Target Code Generation Readme
## Matt McFarland and Yondon Fu - Delights (CS 57 - 16W)
(For more enjoyable reading, please see https://github.com/MattRMcFarland/myCompiler/tree/master/Target_Code)

## General Overview
We've implemented a compiler that can generate Y86 exectuable for a subset of set. The subset of C includes the follow features... 

## Files Structure
The file structure our our compiler is as follows:

We added a source directory to manage the increasing number of source files we created:
* `src/ast.h` and `src/ast.c` : AST nodes
* `src/y86_code_gen.h` and `src/y86_code_gen.c` : Quad Translation
* `src/IR_gen.h` and `src/IR_gen.c` : CG (CodeGenerate) functions
* `src/temp_list.h` and `src/temp_list.c` : Temp generation
* `src/check_sym.h` and `src/check_sym.c` : Top-down type-checking
* `src/symtab.h` and `src/symtab.c` : Symboltable functions
* `src/types.h` : Global types and structure file
* `src/toktypes.h` : Token strings
* `src/ast_stack.h` and `src/ast_stack.c` : AST stack (for scope checking)

Top - Level Files
* `build_ys.sh` : Compile and run a `.c` file
* `scan.l` : flex file
* `parser.y` : bison parsing file 
* `Makefile` : create `./gen_target_code` which generates the `.ys` file
* `y86_code_main.c` : source for generation of target code (`.ys` file)

Instructions for running tests:

`cd Target_Code`

`./build_ys.sh tests/<input_file_name> <output_file_name> [Optional: -g]`

## Implementation Specifics

For the final submission of the compiler, we simply ironed out the bugs from the last milestone of the project (generation of target code). Major refactors are listed here:
* Changing the way target code is printed. Before, we returned strings from functions that would identify an operands source and destination. Passing around static strings was an issue, so we pushed the printing of the target code to `get_source_value()` and `get_dest_value()`. This simplified the quad translation by a lot. 
* Altering Prolog and Post Return. We realized that we don't actually keep track of how many things are pushed onto the stack for a function call because we don't need to save and restore registers between function calls (because values never live in registers between quads). So when a function returns, we manually reset the stack pointer to where it ought live just below the temps and locals for the caller. Before we implemented this reset, we were encountering a lot of off-by-one stack returns. 
* Saving the return value in a temp. In the Post Return quad, we now save the returned value in %eax register to a temp. Without saving %eax to a temp in memory, we were clobbering the return value by expecting to use the return value from two function calls as operands in an expression. Such as : `int a = my_func(1) + my_func(2)`.

## Extra Features

### Sizeof()

### Break and Continue

### Post Increment and Post Decrement



## Testing Files
All test files live in the `tests/` directory. There are three new subdirectories have been added there:
* `edge_cases/`: Includes the stress-tests provided by instructors
* `extra_features/`: Includes test files that demonstrate our extra features
* `my_stress_tests/`: Includes some functions and files that push the compiler

Important test files are highlighted below. Most of the files in `tests/` are rudimentary tests.

### `edge_cases/error.c`
This file highlighted a contested "dark corner" of our formal grammar. Although `gcc` does not compile this file because it prevents the assignment to expressions, our compiler does not throw an error for this file. Our compiler interprets `x + y = 2 + 3` as `x + (y = 2 + 3)` (which `gcc` can compile). We decided not to alter the grammar that was given to us and to default to the latter interpretation of expressions. The `%right` associativity given to the `=` token means that our parser will default to the latter understanding of assignments in the context of other operations. The only way to prevent this without altering the grammar would be to ban the use of assignments as operands to other expressions, which we believe would result in a "less correct" compiler than the difference between `x + y = 2 + 3` as `x + (y = 2 + 3)`.

### `extra_features/inc_dec.c`
This file illustrates how the post increment and post decrement operations work. Note: We encountered an interesting issue here when trying to doubly increment or decrement (`((a)++)++`). This fails our test file, but it also fails in `gcc` because you cannot increment the value of an expression. Only variables can be incremented / decremented because the value must be stored somewhere and gcc doesn't allow for the assignment to an expression.

### `extra_features/sizeof.c`
Shows all the use cases of sizeof. Highlights include:
* `sizeof(local_var)` returns the size of that variable (works for global ints too).
* `sizeof(int param)` returns the size of an int parameter.
* `sizeof(local_arr)` returns the total size of the array (works for global arrays too).
* `sizeof(local_arr[index])` returns the size of an element in the array (works for global arrays too).
* `sizeof(param_arr[index])` returns the size of an element in the parameter array.
* `sizeof(int param_arr[])` produces a syntax error -- the size of the array passed to the function is unknown until runtime, and thus sizeof could only return the size of the array handle. Since that is essentially a pointer (something we don't support), we decided to disallow that request since pointers aren't included in our support set of C.

### `extra_features/tbreakcontinue.c` 
Testing the functionality of break and continue in various for loops and while statements. 

### `my_stress_tests/factorial.c`
Factorial implementation with some logic. This can compute the factorial of the numbers 0 - 13 (too larger means overflow).

### `my_stress_tests/recurse.c`
Simple recursion fuction. Prints decrement input until zero.

### `my_stress_tests/sort.c`
This is an implementation of the selection sort algorithm with swap-in-place and pass array by value to the sorting function (since the sort function can manipulate values in the array without copying them). There's a lot of looping, conditional testing and array manipulation that occurs here.

### `my_stress_tests/test_simple.c` 
This test stretches the parsing, type checking and AST tree creation. This file makes A LOT of temps.

### `tops.c`
Exhaustively tests all operatons and expected outputs.

### `tcond.c`
This files contains many loop iteration tests and conditional tests. It's also fun to watch it run.

### `array.c`
This file tests how local, global and parameter arrays are handled by manipulating values and then printing them to the terminal. We used this file to iron out issues with passing arrays as paramters as well smooth out prologs and epilogs.

### `tnested.c`
Tests scopes and variable shadowing.

### `nestarray.c`
Originally, we had trouble evaluating arrays indexed by other arrays. This file demonstrates how we now evaluate indices and array values from the interior to the outermost array.

### `tfunc.c`
Tests the different ways that function can be declared and called (void parameters, void returns, called without args).

### `nomain.c`
Shows how our compiler will report an error if no `main` function is declared.






