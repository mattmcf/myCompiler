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

extern int yyparse(); 
extern int yydebug; 

ast_node root = NULL;
int parseError = 0; 	      // global flag
int type_error_count = 0;   // used to count type errors
int node_count = 0;         // used to give unique node IDs
quad_arr * quad_list = NULL;    // global quad list
temp_list * temps_list = NULL; // global temp list


int main(void) {
  int noRoot = 0;		/* 0 means we will have a root */
  symboltable_t * symtab;

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

    // printf("****** PRETTY PRINTING SYMBOLTABLE ******\n");
    // print_symtab(symtab);

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
    temps_list = init_temp_list();
    CG(root);
    // call CG on root to fill quad_list
    print_quad_list();
    destroy_quad_list();
  }

  return 0;
}
