# COSC57 Compilers: Lexical Analyzer
## Yondon Fu and Matt McFarland - Delights (CS57 - 16W)

## General Overview
The structure of our lexer is as follows:

* `scan.l` is our flex file with pattern specifications.
* `toktypes.h` is a header file definining the types of tokens for the lexer.
* `cscan.c` is a basic test runner that will print out the output tokens for a input file. We only used this for early stage testing. (From SWS.) To create the "scan" executable (the default executable in the Makefile), simply run "make".
* `scanner_test.c` is a more rigorous test runner that will use an input and expected output file to determine whether token output from the lexer matches the expected output. After the early stage testing, we primarily used this runner for all other testing. This file contains the `main` function used in our tests.
* `mytest.sh` is a bash script file that automates lexer creation and testing using `scanner_test.c`.

Instructions for running tests:

`cd lexical_analyzer`

`./mytest.sh`

## Implementation Specifics
Our lexer follows a similar structure to the demo provided by SWS with some tweaks in certain areas to reflect certain implementation details we deemed appropriate.

### Bad Ids
We define a bad id as a sequence of characters that begins with a digit followed by any number of digits or letters or underscores. This contrasts with a valid id which is a sequence of characters that begins with a letter or underscore followed by any number of letters or digits or underscores. We wanted to ensure that bad id's were not interpreted as integers, underscores and then valid id's separately so we used a macro definition in `scan.l` to recognize these bad id's and classify them with a OTHER_T token.

### Signed and Unsigned Integers
We support signed and unsigned integer syntax in our lexer. Although we only have an INT token and do not have specific signed and unsigned integer tokens, we recognize integers with a + or - symbol directly in front of them as valid INT token. The C library function `atoi` accepts string arguments of this format and can convert them into integers, so we decided to accept integers in this format as well.

### String Scanning
We use a string start condition for string scanning. When the lexer finds a " character, it will switch to the string state. While in this state, anything besides a new line character and an end quote character will be added to yytext. If an escaped double quote is found, it is just concatenated to yytext. If a new line character is found, the string is invalid because there is no end quote and we do not support multi-line strings. Consequently an OTHER_T token is returned after the lexer switches out of string state. If an end quote character is found, the lexer switches out of string state and returns a STRING_T token.

### Comment Gobbling
We handle both single line comment gobbling and multi-line comment gobbling.

For single line comment gobbling we use the SINGLECOMMENT start condition. When the pattern "//" is found, the lexer switches to the SINGLECOMMENT state. While in this state, anything besides the new line character is skipped. When a new line character is found, the lexer switches out of the SINGLECOMMENT state.

For multi-line comment gobbling we use the MULTICOMMENT start condition. When the pattern "/*" is found, the lexer switches to the MULTICOMMENT state. While in this state, anything besides the "*/" is skipped. When the "*/" pattern is found, the lexer switches out of the MULTICOMMENT state.

## Testing Files
All test files live in the tests directory

## Token_Test
### token_test_src.txt and token_test_ans.txt
This file manually steps through all possible tokens and some edge cases.

## Hallway
### hallway.c and hallway_tok.txt
Sample c program that tests a lot of nested for loops statements.

## Print_Matrix
### print_matrix.c and print_matrix_tok.txt
Sample c program that tests a lot of comparision and if statements.


