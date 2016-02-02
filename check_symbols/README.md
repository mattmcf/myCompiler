# Check_Symbols Readme
## Matt McFarland and Yondon Fu - Delights (CS 57 - 16W)
(For more enjoyable reading, please see https://github.com/MattRMcFarland/myCompiler/tree/master/symbol_table)

## General Overview
The file structure our our symbol table generator is as follows:

We structured our file structure a little bit to more conveniently handle the source files.
* `src/`
**


* `scan.l` is our Flex scanner file. It is unchanged from the parser segment of the assignment.
* `parser.y` is our bison file. We had to slightly correct its behavior for compound statement reductions since the parser assignment submission.
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

## Implementation Specifics

Because our AST traversal function acts recursively, we can define the appropriate actions for symbols defined in terms of the node (root) being currently visited. (These actions can be seen in the `traverse_ast_node` function in symtab.c) We realized that there were only three ast nodes that we need to attend to in filling out the symbol table. They are enumerated below. In general, our algorithm looked like this: 

```
Push root onto current scope's stack

Get root's ast node type

Handle root for it's type (described below -- add symbols or enter new scope)

For each child
	traverse child

Pop current scope stack
If stack is empty and there are no sibling nodes
	Exit scope

Return
```

* Handling a function declaration node:

This ended up being the most conceptually difficult symbol to add. We first went through the function nodes children to collect it's return type, symbol ID, and arguments (the arguments were stored in terms of a new structure called a 'variable' which contains an ID, a type and a datatype modifier [array or single instance]). The function symbol was added to the current scope. Then we entered a new scope manually and added each argument to the new scope's symbols. We then returned the first child in the function's compound statement block to begin a horizontal traversal. (We have to return the child of the compound statement node to avoid entering another new scope.) 


* Handling a variable declaration line node:

This node was fairly simple to handle. We look for the type specifier in the left most child and then went through the children to add symbol IDs and modifiers (arrays or single instances). This handling function does not return any new ast node to continue traversal on because new scopes cannot exist under a variable declaration node.

* Handling a compound statement node:

This node triggers the entrance of a new scope.

* Logic for exiting scope

We built an AST stack to help the recursive tranversal function know when to exit scope. Each node in the current scope is pushed onto the stack, and once all nodes are popped and there are no further possible nodes to visit in the current scope, the scope should be exited. When there are no items on the top level stack and no further nodes to visit, the traversal is finished.

## Testing Files
All test files live in the tests directory

We hand wrote our answer key files by manually going through input files and constructing what we thought should be the expected symbol table structure for a particular file. We then piped our output into a .out file which we diffed with our key file to check if the actual output matched our expected output.

### tcurly.c and tcurly_ans.txt
Sample C program with numerous nested block scopes and variable declarations within them.

### tscope.c and tscope_ans.txt
Sample C program with multiple function scopes, as well as edge cases such as blocks with a single semi-colon, empty functions without any variable declarations. We do not reflect this in our key file, but we can also test for duplicate variable declarations by uncommenting line 21 in tscope.c. The output will be that a duplicate variable has been found and the symbol table program will stop running.

Note: other test input files without key files can also be found in the tests directory.



