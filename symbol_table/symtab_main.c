/*
 * FILE: symtab_main.c
 * DESCRIPTION: builds a parse tree and symbolic table
 *
 * Originally skeleton Written by THC for CS 57; massaged by SWS. 
 * Students: Yondon Fu and Matt McFarland - Delights (CS 57 - 16W)
 *
 */

#include <stdio.h>
#include "ast.h"

ast_node root = NULL;

extern int yyparse(); 	// bison's generated file?
extern int yydebug; 	// defined in parser.y
int parseError = 0; 	// global flag

int main() {
  int noRoot = 0;		/* 0 means we will have a root */

  //yydebug = 1;
  noRoot = yyparse();

  if (parseError)
    fprintf(stderr, "WARNING: There were parse errors.\nParse tree may be ill-formed.\n");

  if (!noRoot) {
  	print_ast(root,0);
  	
    /* create sym table here? */

    /* Always insert at the innermost scope within symbol table
     * If we encounter a {, we call enter_scope, which will create
     * a new hash table for the innermost scope. All following insert
     * calls will insert into the hash table representing the innermost scope.
     * When we encounter a }, we call leave_scope, which will move us up
     * one level in the symbol table such that we are looking at a scope that is
     * one level up.
     */


    // just plug in things we declare -- DON'T NEED TO CHECK (for now)	
    // declaring two id's is an error -- that we should catch
  }


  return 0;
}
