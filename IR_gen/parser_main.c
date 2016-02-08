/*
 * parser_main.c
 *
 * builds and prints a parse tree
 *
 * Written by THC for CS 57; massaged by SWS.  Modify this file as appropriate.
 *
 * Students: Yondon Fu and Matt McFarland - Delights (CS57 16W)
 * 
 */

#include <stdio.h>
#include "src/ast.h"
#include "src/ast_stack.h"

ast_node root = NULL;

extern int yyparse(); 	// bison's generated file?
extern int yydebug; 	// defined in parser.y
int parseError = 0; 	// global flag

int FillASTstack(ast_node root, ast_stack * stk);

int main() {
  int noRoot = 0;		/* 0 means we will have a root */

	ast_stack *s = InitASTStack(10);

  //yydebug = 1;
  noRoot = yyparse();

  if (parseError)
    fprintf(stderr, "WARNING: There were parse errors.\nParse tree may be ill-formed.\n");

  if (!noRoot) {
  	print_ast(root, 0);
  	if (FillASTstack(root, s))
			printf("failed to fill stack\n");
  }
  
  //printf("stack has %d things on it right now...\n",ASTSize(s));
 
  ast_node popped;
  while ( (popped = ASTPop(s)) != NULL) {
  	//printf("popped! %s \n", NODE_NAME(popped->node_type));
    ;
  }
  	
  DestroyASTStack(s);

  return 0;
}

int FillASTstack(ast_node root, ast_stack * stk) {

	if (root == NULL || stk == NULL) {
		fprintf(stderr,"null root / null stack\n");
		return 1;
	}

  ast_node pushed = ASTPush(root,stk);
	//printf("pushed %s\n", NODE_NAME(pushed->node_type));

	/* Recurse on each child of the subtree root */
  ast_node child;
  for (child = root->left_child; child != NULL; child = child->right_sibling) {
  	FillASTstack(child, stk);
  	//printf("filling stack with %s\n", NODE_NAME(child->node_type));
  }
  
	return 0;
}
