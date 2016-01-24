# COSC57 Compilers: Parser
## Yondon Fu and Matt McFarland - Delights (16W)
(For more enjoyable reading -> https://github.com/MattRMcFarland/myCompiler/tree/master/parser)


## General Overview
The structure for our parser is as follows:

* `scan.l` is our scanner file. It has been updated since submitting the lexer assignment.
* `toktypes.h` is our header file defining the types of tokens for the lexer. It has been updated since submitting the lexer assignment. (Note: This token file is useless since all the tokens are declared in the `parser.y`. `toktypes.h` is just used for pretty printing of token names.)
* `ast.c` contains functions for creating and printing AST nodes
* `ast.h` is our header file defining AST nodes and their possible types
* `parser.y` is the bison file for our parser
* `parser_main.c` is our driver program that runs our parser to build and then print the abstract syntax tree
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

#### On our general approach 
We used the Abstract Syntax Tree skeleton model that was provided in the sample code from SWS. We have added many new nodes to handle the expanded subset of C however. Most of these nodes have self-explanatory and intuitive structures (ie, if a node type is only to have 1 child, then that child will live at the Left Child). However there are some complicated / subtle cases where we made some design decisions about structural nodes with multiple possible children components.

In general, the relationship between children node order and production rule is as such:

* Where t is the current node...
* First Component: t -> left child
* Second Component: t -> left child -> right sibling
* Third Component: t -> left child -> right sibling -> right sibling
* ... and so on.

So we tried to follow an overarching design principle that structural nodes without children should be NULL. This organizational system falls apart when one of the subcomponent nodes is NULL. If you try to access the right sibling of a NULL node to string on the next component node, bad things happen. For example:

```{ 
	int i = 10; 
}```

and 

```int i;
{ 
	i = 10; 
}```

In the first case, the production for compound statement sees that the there are no statements in the stmt_list, and so the right sibling of the local_declaration list is NULL. No problems there.

In the second case however, there are statements but no local declarations. So if we stick to the convention that the first subcomponent is the left child and the second is the left child -> right sibling, we will set the left child to NULL and attempt to set NULL -> right sibling to the stmt_list. Problems! So to handle weird cases like this, we have decided that empty subcomponents should be skipped and all the right siblings moved over one. 

Each node's exact implementation details are in the comments above each production rule invovling that node's creation. Look at parser.y for these notes. I hesitate to add the structural details here for every node because have two sources of documentation for the nodes will be hard to keep straight. In any case, there are only a few subtle cases to look out for anyways (compount statements (rule 13), loop declarations, and function declarations (rule 8)). 

Note: When the parser encounters `{}`, it will see an empty declaration list and an empty statement list and thus evaluate that compound statement list as NULL (removing it from the tree essentially).

#### On node structure:
For some nodes with different meanings / computational consequences, the structure of the node's children define which operational variant that node is. For example, varable declarations such as `int a` and `int arr[]` should be handled differently (as in given different nodes) because these two declarations imply computational differences in execution. These details are documented in the parser.y file above each rule production. 

Example: Rule 7 for variable declarations where the number and type of children define what kind of variable / assignment operation occurs.

### Error Handling
We used Bison's special error token within certain productions to catch syntax errors and report them along with the line number that they occur on.

Our parser reports syntax errors and continues parsing until it either finishes parsing or the number of errors exceeds a max error variable that we set in parser.y. If the parser finishes parsing, the parser will output the syntax error reports along with the parsed tree structure. However, if there are syntax errors reported, there are no guarantees as to the validity of the parse tree output. If the number of errors exceeds the max error value, the parser will abort and inform the user of the syntax errors found up until the point of abortion. 

### Testing Files
All test files live in the tests directory

Note: due to the large size of parse trees generated by our parser, entirely writing our expected output key files was impractical. So, instead, we first generated an output file for a source file and then manually went through the output file to check the validity of the outputted parse tree. We copied over parts of the parse tree that we believed were valid to the key file. Any parts that we though were wrong, we would edit by hand and then copy over the edited output into the key file. Consequently, we ended up with key files that reflected what we expected the output to be. We thought this process was more practical than trying to write out expected output by hand.

#### t1.c and t1.key
Sample C program that includes a basic single line variable declaration.

#### test_parse.c and test_parse.key
Sample C program that includes various syntantic structures that would appear in a typical program written in the C subset we are using.

#### list_decl.c and list_decl.key
Sample C program includes some edge cases for our syntax tree structure using variable list declaration nodes.

#### test_err.c
Sample C program that includes the following syntax errors: consecutive operators, an operator followed by parentheses and dangling else. It also includes such errors within different types of statements such as for, while and do-while loops as well as return, print and read statements.