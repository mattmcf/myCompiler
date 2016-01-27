/*
 * tree_main.c
 *
 * Driver program for a parser that builds an abstract syntax tree.
 *
 * Written by THC for CS 57; massaged by SWS.  Modify this file as appropriate.
 *
 */

#include <stdio.h>
#include "ast.h"
#include "ast_stack.h"

ast_node root = NULL;

extern int yyparse(); 	// bison's generated file?
extern int yydebug; 	// defined in parser.y
int parseError = 0; 	// global flag

int FillASTstack(ast_node root, ast_stack * stk);

int main() {
  int noRoot = 0;		/* 0 means we will have a root */

	ast_stack *s = InitASTstack(10);

  //yydebug = 1;
  noRoot = yyparse();

  if (parseError)
    fprintf(stderr, "WARNING: There were parse errors.\nParse tree may be ill-formed.\n");

  if (!noRoot) {
  	print_ast(root, 0);
  	if (FillASTstack(root, s))
			printf("failed to fill stack\n");
  }
  
  printf("stack has %d things on it right now...\n",s->top-2);

  ast_node popped = ASTPop(s);  
  while (popped != NULL) {
  	printf("popped! %s \n", NODE_NAME(popped->node_type));
  	popped = ASTPop(s);
  }
  	

  DestroyASTstack(s);

  return 0;
}

int FillASTstack(ast_node root, ast_stack * stk) {

	if (root == NULL || stk == NULL) {
		fprintf(stderr,"null root / null stack\n");
		return 1;
	}

	printf("pushed %s\n", NODE_NAME(ASTPush(root,stk)->node_type));

	/* Recurse on each child of the subtree root */
  ast_node child, pushed;
  for (child = root->left_child; child != NULL; child = child->right_sibling) {
  	FillASTstack(child, stk);
  	//printf("filling stack with %s\n", NODE_NAME(child->node_type));
  }
  
	return 0;
}
