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

    Potential Psuedocode

    // if node is null:
    //   return // Base Case

    // if node is scope start node:
    //   enter_scope(symboltable, node)
    // elif length of symboltable.leaf.scopeStack == 0:
    //   leave_scope(symboltable)
    // else:
    //   insert node into symboltable.leaf
    //   ASTPush(symboltable.leaf.scopeStack, node)

    // astnode = root.left_child
    // for right sibling of astnode:
    //   recurse
    //   ASTPop(symboltable.leaf.scopeStack) // Floating back up, pop node off

    // could we do this in the following? -- or does that not work with recursive call?
    // how to implement a "get next node" function? 
    ast_node cur;
    while ( cur = get_next_node() != NULL ) {
      handle_node(cur, symtab);
    }

     


    // just plug in things we declare -- DON'T NEED TO CHECK (for now)	
    // declaring two id's is an error -- that we should catch
  }


  return 0;
}

/*
 * handle_ast_node(ast_node n, symboltable_t)
 */
int handle_ast_node(ast_node n, symboltable_t symtab) {

  assert(n);
  assert(symtab);

  /* get node type */
  switch(n->node_type) {
    case VAR_N;
      // don't need to handle?
      break;

    case FOR_HEADER_N:
      // add declared variables to new scope declared for FOR_STMT_N
      break;

    case FOR_STMT_N:
      // new scope
      // add all children to new scope
      break;

    case DO_WHILE_N:
      // new scope
      // add all children to new scope
      break;

    case WHILE_N:
      // new scope
      // add all children to new scope
      break;

    case COMPOUND_STMT_N:
      // new scope
      // add all children to new scope
      break;

    case FORMAL_PARAM_N:
    case FORMAL_PARAM_ARR_N:
    case FUNC_DECLARATION_N:
      // add function declaration symbol to leaf (global)
      // new scope
      // add arguments (formal params) to new scope
      break;

    case VAR_DECLARATION_N:
      // for each VAR_DECL_N child, add to current scope
      //    TYPE is left most child
      break;

    case VAR_DECL_N:
      // add to current scope -- handled in VAR_DECLARATION_N?
      break;

    case IF_STMT_N:   // confused about this one
      // new scope ?
      break;

    case IF_ELSE_STMT_N:
      // new scope ?
      break;

    default:
      /* don't care */
      break;
  }

  // done?
  return 0?
}
