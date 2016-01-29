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

/* 
 * get_type() : returns enumerated type_specifier (definied in symtab.h)
 * for an ast_node n input
 * 
 * returns NULL_TS is node isn't of recognized type
 */
type_specifier get_datatype(ast_node n);

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

    //Potential Psuedocode

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

void traverse_ast_tree(ast_node root, symboltable_t * symtab) {

  assert(symtab);

  if (root == NULL)
    return;

  // -------

  // ASTPush(symboltable.leaf.scopeStack, root)

  // handle root
    // switch(root->node_type)
    // case FDL
      // add Function declaration to current scope
      // enter new scope
      // add all parameters to new scope
      // go to CS->
      // for all children of CS
        // traverse_ast_tree child
      
    // case VDL
      // for each child, add child symbol to current scope
      // for each right sibling of VDL
        // traverse sibling

    // case CS
      // Enter new scope
      // for all children
        // traverse_ast_tree child

    // default
      // for all children
        // traverse_ast_tree child

  // ASTPop(symboltable.leaf.scopeStack)

  // if stack size == 0
    // exit scope 

  // return

  // -------

  /* Recurse on each child of the subtree root, with a depth one
     greater than the root's depth. */

  // use this for loop
  ast_node child;
  for (child = root->left_child; child != NULL; child = child->right_sibling)
    print_ast(child, depth + 1);

  // -------

    // if node is scope start node:
    //   enter_scope(symboltable, node)
    // elif length of symboltable.leaf.scopeStack == 0:
    //   leave_scope(symboltable)
    // else:
    //   insert node into symboltable.leaf



  // astnode = root.left_child
  // for right sibling of astnode:
  //   recurse
  //   ASTPop(symboltable.leaf.scopeStack) // Floating back up, pop node off
}

/*
 * handle_ast_node(ast_node n, symboltable_t)
 */
int handle_ast_node(ast_node n, symboltable_t symtab) {

  assert(n);
  assert(symtab);

  /* get node type */
  switch(n->node_type) {
    case VAR_N;   // HOLD OFF, NECESSARY?
      // check if n->left_child->value_string is declared variable symbol
      // n->left_child->right_sibling will be NULL if simple variable
      // n->left_child->right_sibling will be not null if an array variable
      break;

    case FOR_STMT_N: // HOLD OFF, NECESSARY?
      // first 3 children are FOR_HEADER_NODES, each will exist, but could be empty
      // if (has_fourth_child && fourth_child == COMPOUND_STMT_N)
      //    enter new scope for attached compound statment / expression statement
      //    note: you can't declare a new variable in a 1-liner EXPRESSION_STMT following a for statement
      break;

    case DO_WHILE_N: // HOLD OFF, NECESSARY?
      // do_while must have a compound statement node as the left_child
      // but it could be empty and not have any children.
      // if (n->left_child->leftchild != NULL)
      //    new scope
      //    dive into compound statement...
      break;

    case WHILE_N: // HOLD OFF, NECESSARY?
      // if (n->left_child->right_sibling == compound statement)
      //    new scope
      //    dive into compound statement
      break;

    case IF_STMT_N:  // HOLD OFF, NECESSARY?
      // new scope ?
      break;

    case IF_ELSE_STMT_N: // HOLD OFF, NECESSARY?
      // new scope ?
      break;

    case COMPOUND_STMT_N:
      // new scope
      //    add children to new scope
      break;

    case FORMAL_PARAM_N:
    case FORMAL_PARAM_ARR_N:
    case FUNC_DECLARATION_N:
      // add function declaration symbol to leaf (global)
      // n->left_child->left_child is type_N
      // n->left_child->right_sibling is ID_N
      //
      // enter new scope
      // if (n->left_child->right_sibling->right_sibling != VOID_N), then it
      //    is a FORMAL_PARAM_N with children as FORMAL_PARAMETER / FORMAL_PARAMETER_ARR list of arguments
      //    add these arguments to the new scope
      // get (n->left_child->right_sibling->right_sibling->right_sibling)
      // if ^^^ is a compound statement with children, go to FIRST CHILD
      // and start add variables to new scope there.
      //
      // we need to skip the Compound statement because we've already
      // entered a new scope.
      break;

    case VAR_DECL_N:  
    case VAR_DECLARATION_N:
      // type_specifier t = get_type(n->left_child)
      // for each VAR_DECL_N child, add to current scope
      //    n->left_child is a type node
      //    ast_node c = n->left_child->right_sibling
      //    while (c != NULL)
      //      if(c->left_child->right_sibling != NULL && c->left_child->right_sibling == INT_LITERAL_N)
      //         add c as an array variable INT_TS
      //      else
      //          add c as a regular variable 
      break;

    default:
      /* These aren't the symbols you're looking for. */
      break;
  }

  // done?
  return 0?
}


/* 
 * returns enumerated type_specifier (definied in symtab.h)
 * for an ast_node n input
 */
type_specifier get_datatype(ast_node n) {

  assert(n);

  type_specifier t;
  switch (n->node_type) {
    case TYPEINT_N:
      t = INT_TS;
      break;

    case VOID_N:
      t = VOID_TS;
      break;

    default:
      t = NULL_TS;  // not a valid type
      break;
  }

  return t;
}


/*
 * is_array is 1 if variable is array
 */
 add_var_symbol(char * id, type_specifier_t type, int is_array, symboltable_t * symtab);

 /*
  * free arg_types array once done with function call
  */
add_func_symbol(char * id, type_specifier_t return_type, int arg_count, type_specifier * arg_types, symboltable_t * symtab)