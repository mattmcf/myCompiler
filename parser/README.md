# COSC57 Compilers: Parser
## Yondon Fu and Matt McFarland - Delights (16W)

## General Overview

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
Sample C program that includes the following syntax errors: consecutive operators, an operator followed by parentheses and dangling else.