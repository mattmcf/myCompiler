# COSC57 Compilers: Parser
## Yondon Fu and Matt McFarland - Delights (16W)

## General Overview
The structure for our parser is as follows:

* `scan.l` is our scanner file. It has been updated since submitting the lexer assignment.
* `toktypes.h` is our header file defining the types of tokens for the lexer. It has been updated since submitting the lexer assignment.
* `ast.c` contains functions for creating and printing AST nodes
* `ast.h` is our header file defining AST nodes and their possible types
* `parser.y` is the bison file for our parser
* `parser_main.c` is our driver program that runs our parser to build an abstract syntax tree
* `mytest.sh` is a bash script file that automates parser creation and testing by generating output files in the results directory and diffing them with the expected output key files in the tests directory.

Instructions for running tests:

`cd parser`

`./mytest.sh`

Instructions for running tests for error reporting:

`cd parser`

`make clean && make`

`./parser < tests/test_err.c`

Output will include syntax error reporting by bison as well as any additional output from our parser whether it be an attempt at generating a parse tree or a message alerting the user that the parser has aborted due to too many errors.

## Implementation Specifics

### Error Handling
We used Bison's special error token within certain productions to catch syntax errors and report them along with the line number that they occur on.

Our parser reports syntax errors and continues parsing until it either finishes parsing or the number of errors exceeds a max error variable that we set in parser.y. If the parser finishes parsing, the parser will output the syntax error reports along with the parsed tree structure. However, if there are syntax errors reported, there are no guarantees as to the validity of the parse tree output. If the number of errors exceeds the max error value, the parser will abort and inform the user of the syntax errors found up until the point of abortion. 

### Testing Files
All test files live in the tests directory

#### t1.c and t1.key
Sample C program that includes a basic single line variable declaration.

#### test_parse.c and test_parse.key
Sample C program that includes various syntantic structures that would appear in a typical program written in the C subset we are using.

#### list_decl.c and list_decl.key
Sample C program includes some edge cases for our syntax tree structure using variable list declaration nodes.

#### test_err.c
Sample C program that includes the following syntax errors: consecutive operators, an operator followed by parentheses and dangling else. It also includes such errors within different types of statements such as for, while and do-while loops as well as return, print and read statements.