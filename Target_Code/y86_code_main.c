/*
 * FILE: IR_gen.c
 * DESCRIPTION: builds intermediate representation of code
 *
 * Originally skeleton Written by THC for CS 57; massaged by SWS. 
 * Students: Yondon Fu and Matt McFarland - Delights (CS 57 - 16W)
 *
 */

#include <stdio.h>
#include "src/ast.h"
#include "src/symtab.h"
#include "src/check_sym.h"
#include "src/IR_gen.h"
#include "src/y86_code_gen.h"

extern int yyparse(); 
extern int yydebug; 

ast_node root = NULL;
int parseError = 0; 	      // global flag
int type_error_count = 0;   // used to count type errors
int node_count = 0;         // used to give unique node IDs
quad_arr * quad_list = NULL;    // global quad list
symboltable_t * symtab;

int main(void) {
  int noRoot = 0;		/* 0 means we will have a root */

  //yydebug = 1;
  noRoot = yyparse();

  if (parseError)
    fprintf(stderr, "WARNING: There were parse errors.\nParse tree may be ill-formed.\n");

  if (!noRoot && !parseError) {
  	//print_ast(root,0);
  	
    /* create empty symboltable */
    symtab = create_symboltable();
    if (!symtab){
      fprintf(stderr, "couldn't create symboltable\n");
      return 1;
    }

    /* fill symbol table up */
    traverse_ast_tree(root, symtab);

    /* check types */
    set_type(root);
    if (type_error_count != 0) {
      fprintf(stderr,"%d type errors found. Please fix before continuing.\n",type_error_count);
      return 1;
    }

    printf("\n\n ----- PRETTY PRINTING AST TREE WITH TYPES -----\n");
    print_ast(root,0);  

    /* Start to generate quads */
    quad_list = init_quad_list();
    CG(root);

    printf("\n\n ----- PRINTING QUAD LIST -----\n");
    print_quad_list();

    printf("\n\n ----- PRETTY PRINTING SYMBOLTABLE WITH TEMP VARIABLES -----\n");
    print_symtab(symtab);

    create_ys("my_file");

    // clean up
    destroy_quad_list();
  }

  return 0;
}
