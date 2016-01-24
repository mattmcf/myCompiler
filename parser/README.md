# COSC57 Compilers: Parser
## Yondon Fu and Matt McFarland - Delights (16W)

## General Overview

## Implementation Specifics

### Error Handling
We used Bison's special error token within certain productions to 

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