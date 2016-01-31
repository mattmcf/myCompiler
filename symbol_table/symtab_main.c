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
#include "symtab.h"

ast_node root = NULL;

extern int yyparse(); 
extern int yydebug; 
int parseError = 0; 	// global flag

int main(void) {
  int noRoot = 0;		/* 0 means we will have a root */
  symboltable_t * symtab;

  //yydebug = 1;
  noRoot = yyparse();

  if (parseError)
    fprintf(stderr, "WARNING: There were parse errors.\nParse tree may be ill-formed.\n");

  if (!noRoot) {
  	//print_ast(root,0);
  	
    /* create empty symboltable */
    symtab = create_symboltable();
    if (!symtab){
      fprintf(stderr, "couldn't create symboltable\n");
      return 1;
    }

    /* fill it up */
    traverse_ast_tree(root, symtab);

    /* print it out */
    printf("****** PRETTY PRINTING SYMBOLTABLE ******\n");
    print_symtab(symtab);
    
  }

  return 0;
}

