# COSC57 Compilers: Parser
## Yondon Fu and Matt McFarland - Delights (16W)
(For more enjoyable reading -> https://github.com/MattRMcFarland/myCompiler/tree/master/parser)


## General Overview

## Implementation Specifics

#### On our general approach 
We used the Abstract Syntax Tree skeleton model that was provided in the sample code from SWS. We have added many new nodes to handle the expanded subset of C however. Most of these nodes have self-explanatory and intuitive structures (ie, if a node type is only to have 1 child, then that child will live at the Left Child). However there are some complicated / subtle cases where we made some design decisions about structural nodes with multiple possible children components.

In general, the relationship between children node order and production rule is as such:

Where t is the current node...
First Component: t -> left child
Second Component: t -> left child -> right sibling
Third Component: t -> left child -> right sibling -> right sibling
... and so on.

So we tried to follow an overarching design principle that structural nodes without children should be NULL. Where this organizational system falls apart is if one of the subcomponent nodes is NULL. When you try to access the right sibling of a NULL node, bad things happen. For example:

`{
	int i = 10;
}`

and 

`int i;
{
	i = 10;
}`

In the first case, the production for compound statement sees that the there are no statements in the stmt_list, and so the right sibling of the local_declaration list is NULL. No problems there.

In the second case however, there are statements but no local declarations. So if we stick to the convention that the first subcomponent is the left child and the second is the left child -> right sibling, we will set the left child to NULL and attempt to set NULL -> right sibling to the stmt_list. Problems! So to handle weird cases like this, we have decided that empty subcomponents should be skipped and all the right siblings moved over one. 

Each node's exact implementation details are in the comments above each production rule invovling that node's creation. Look at parser.y for these notes. I hesitate to add the structural details here for every node because have two sources of documentation for the nodes will be hard to keep straight. In any case, there are only a few subtle cases to look out for anyways (compount statements (rule 13), loop declarations, and function declarations (rule 8)). 

Note: When the parser encounters `{}`, it will see an empty declaration list and an empty statement list and thus evaluate that compound statement list as NULL (removing it from the tree essentially).

#### On node structure:
For some nodes with different meanings / computational consequences, the structure of the node's children define which operational variant that node is. For example, varable declarations such as `int a` and `int arr[]` should be handled differently (as in given different nodes) because these two declarations imply computational differences in execution. These details are documented in the parser.y file above each rule production. 

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