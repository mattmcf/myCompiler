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

#include <assert.h>     // should move functions to symtab.h

ast_node root = NULL;

extern int yyparse(); 	// bison's generated file?
extern int yydebug; 	// defined in parser.y
int parseError = 0; 	// global flag

/*
//  * is_array is 1 if variable is array
//  *
//  * usually referencing a static variable on stack. copy into memory.
//  *
//  * returns 0 if symbol was successfully added
//  * returns 1 if this symbol was already delclared
//  */
// int add_var_symbol(variable * var, symboltable_t * symtab);

 
//   * free arg_types array once done with function call
//   *
//   * returns 0 if symbol was successfully added
//   * returns 1 if this symbol was already delclared
  
// int add_func_symbol(char * id, type_specifier_t return_type, int arg_count, variable * arg_arr, symboltable_t * symtab);

/* traversal function */
void traverse_ast_tree(ast_node root, symboltable_t * symtab);

/* handle function declaration nodes */
ast_node handle_func_decl_node(ast_node fdl, symboltable_t * symtab);

/* handle variable declaration line nodes */
void handle_var_decl_line_node(ast_node vdl, symboltable_t * symtab);

int main(void) {
  int noRoot = 0;		/* 0 means we will have a root */
  symboltable_t * symtab;

  //yydebug = 1;
  noRoot = yyparse();

  if (parseError)
    fprintf(stderr, "WARNING: There were parse errors.\nParse tree may be ill-formed.\n");

  if (!noRoot) {
  	print_ast(root,0);
  	
    /* create empty symboltable */
    symtab = create_symboltable();
    if (!symtab){
      fprintf(stderr, "couldn't create symboltable\n");
      return 1;
    }

    /* fill it up */
    traverse_ast_tree(root, symtab);
  }

  return 0;
}


void traverse_ast_tree(ast_node root, symboltable_t * symtab) {

  assert(symtab);

  if (root == NULL)
    return;

  ASTPush(root, symtab->leaf->scopeStack);

  switch(root->node_type) {
  case FUNC_DECLARATION_N:
    // handle function node declaration and skip to first child of function compound statement
    for (ast_node child = handle_func_decl_node(root,symtab); child != NULL; 
          child = child->right_sibling) {
      traverse_ast_tree(child, symtab);
    }
      
    break;

  case VAR_DECLARATION_N:
    handle_var_decl_line_node(root,symtab);
    // don't traverse these children
    // can return now because parent call will move onto sibling
    break;

  case COMPOUND_STMT_N:
    if (root->left_child != NULL)
      enter_scope(symtab, root->left_child);

    for (ast_node child = root->left_child; child != NULL; child = child->right_sibling)
      traverse_ast_tree(child, symtab);

    break;

  default:
    for (ast_node child = root->left_child; child != NULL; child = child->right_sibling)
      traverse_ast_tree(child, symtab);

    break;  
  }

  ASTPop(symtab->leaf->scopeStack);

  if (ASTSize(symtab->leaf->scopeStack) == 0)
    leave_scope(symtab);

  return;

  // // -------
  /* old pseudo code */

  // /* Recurse on each child of the subtree root, with a depth one
  //    greater than the root's depth. */

  // // use this for loop
  // ast_node child;
  // for (child = root->left_child; child != NULL; child = child->right_sibling)
  //   print_ast(child, depth + 1);

  // // -------

  //   // if node is scope start node:
  //   //   enter_scope(symboltable, node)
  //   // elif length of symboltable.leaf.scopeStack == 0:
  //   //   leave_scope(symboltable)
  //   // else:
  //   //   insert node into symboltable.leaf



  // // astnode = root.left_child
  // // for right sibling of astnode:
  // //   recurse
  // //   ASTPop(symboltable.leaf.scopeStack) // Floating back up, pop node off
}


/*
 * handle_func_decl_node(ast_node * fdl, symboltable_t * t);
 */
ast_node handle_func_decl_node(ast_node fdl, symboltable_t * symtab) {

  assert(fdl);
  assert(symtab);

  // Insert symnode for function into leaf symhashtable
  symnode_t *fdl_node = insert_into_symboltable(symtab, fdl->value_string);

  if (fdl_node == NULL) {
    /* duplicate symbol in scope */
    fprintf(stderr, "error: duplicate symbol \'%s\' found. Please fix before continuing.\n", fdl->value_string);
    exit(1);
  }

  // Set symnode as a func node
  set_node_type(fdl_node, FUNC_SYM);

  // Get return type
  type_specifier_t return_type = get_datatype(fdl->left_child->left_child);
  if (return_type == NULL_TS) {
    fprintf(stderr, "seeing NULL_TS return type, something wrong\n");
    return NULL;
  }

  // Get id specifier
  char *id = fdl->left_child->right_sibling->value_string;     // should strdup this for the symbol table?

  // Count arguments
  ast_node arg = fdl->left_child->right_sibling->right_sibling;
  assert(arg->left_child);
  arg = arg->left_child;

  int arg_count = 0;
  var_symbol * arg_arr = NULL;

  /* collect all argument parameters */
  if (arg->node_type != VOID_N) {

    int arg_arr_size = 5;     /* magic number */
    arg_arr = (var_symbol *)calloc(arg_arr_size, sizeof(var_symbol));
    assert(arg_arr);
    
    type_specifier_t type;
    modifier_t mod;
    char * name;

    while (arg != NULL) {

      /* make a variable for each argument */
      type = get_datatype(arg->left_child);
      if (arg->node_type == FORMAL_PARAM_N)
        mod = SINGLE_DT;
      else if (arg->node_type == FORMAL_PARAM_ARR_N)
        mod = ARRAY_DT;
      else {
        fprintf(stderr,"what kind of argument node is this???");
        exit(1);
      }

      name = arg->left_child->right_sibling->value_string;

      arg_arr[arg_count] = init_variable(name, type, mod);  // static variable struct on stack 
      arg_count++;

      /* resize type array */
      if (arg_count == arg_arr_size) {
        arg_arr_size *= 2;
        arg_arr = realloc(arg_arr, sizeof(var_symbol) * arg_arr_size);
        assert(arg_arr);
      }

      /* move to next argument */
      arg = arg->right_sibling;
    }
  }

  /* add function symbol to current (global) scope */
  //add_func_symbol(id, return_type, arg_count, arg_arr, symtab);

  // Set fields for func node in current (global) scope
  set_node_func(fdl_node, id, return_type, arg_count, arg_arr);

  /* get CS node */
  ast_node compound_stmt = fdl->left_child->right_sibling->right_sibling->right_sibling;
  ast_node next_node = NULL;

  if (compound_stmt->left_child != NULL) {

    /* enter new scope -- skipping CS node */
    enter_scope(symtab, compound_stmt->left_child);

    /* add function parameters to this new scope symbol table */
    for (int i = 0; i < arg_count; i++) {

      // Insert var node in leaf symhashtable
      symnode_t *var_node = insert_into_symboltable(symtab, (&arg_arr[i])->name);

      if (var_node == NULL) {
        fprintf(stderr, "error: duplicate variable symbol \'%s\' found. Please fix before continuing.\n", (&arg_arr[i])->name);
        exit(1);
      }

      set_node_type(var_node, VAR_SYM);

      set_node_var(var_node, &arg_arr[i]);
      // add_var_symbol(&arg_arr[i], symtab); 
    }

    /* return next node to start traverse on */
    next_node = compound_stmt->left_child;

    // Handle VDL node if present
    //handle_var_decl_line_node(compound_stmt->left_child, symtab);
    //leave_scope(symtab);
  } 

  if (arg_arr != NULL)
    free(arg_arr);

  return next_node;

}


void handle_var_decl_line_node(ast_node vdl, symboltable_t * symtab) {

  assert(symtab);
  assert(symtab);

  type_specifier_t this_type = get_datatype(vdl->left_child->left_child);

  ast_node child = vdl->left_child->right_sibling;

  while (child != NULL) {
    char *name = child->left_child->value_string;
    modifier_t mod;

    if (child->left_child->right_sibling == NULL) {
      mod = SINGLE_DT;
    } else if (child->left_child->right_sibling->node_type == INT_LITERAL_N) {
      mod = ARRAY_DT;
    } else {
      mod = SINGLE_DT;
    }

    // Insert symnode for variable
    symnode_t *var_node = insert_into_symboltable(symtab, name);

    if (var_node == NULL) {
      fprintf(stderr, "error: duplicate variable symbol \'%s\' found. Please fix before continuing.\n", name);
      exit(1);
    }    

    set_node_type(var_node, VAR_SYM);

    // Initialize variable
    var_symbol new_var = init_variable(name, this_type, mod);

    // Set variable field in symnode
    set_node_var(var_node, &new_var);

    // Next variable
    child = child->right_sibling;
  }

  // type_specifier_t this_type = get_datatype(vdl->left_child->left_child);

  // ast_node child = vdl->left_child->right_sibling;
  // variable new_var;

  // while (child != NULL) {

  //   new_var.name = child->left_child->value_string;
  //   new_var.type = this_type;

  //   if (child->left_child->right_sibling == NULL)
  //     new_var.modifier = SINGLE_DT;
  //   else if (child->left_child->right_sibling->node_type == INT_LITERAL_N)
  //     new_var.modifier = ARRAY_DT;
  //   else
  //     new_var.modifier = SINGLE_DT;

  //   if (add_var_symbol(&new_var, symtab)) {
  //     fprintf(stderr, "Variable %s already declared\n",new_var.name); 
  //     exit(1);
  //   }

  //   child = child->right_sibling;
  // }
}

// /*
//  * #########
//  * OLD STUFF and psuedo code 
//  * #########





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
    // ast_node cur;
    // while ( cur = get_next_node() != NULL ) {
    //   handle_node(cur, symtab);
    // }

    // // FDL Psuedocode
    // if node_type == FDL:
    //   n = insert_into_symtable(symtab, "FDL")
    //   set_node_type(n, func)

    //   set_type(n, n->left_child->left_child)
    //   set_name(n, n->left_child->right_sib->value_string)

    //   // Formal params
    //   enter_scope()
    //   formal_param_n = n->left_child->right_sib->right_sib

    //   int count = 0
    //   for (curr = n->left_child; curr->right_child != null; curr = curr->right_child):
    //     count++
    //     inserted = insert_node_into_symtable(symtab, "Param")
    //     set_node_type(inserted, var)
    //     set_type(inserted, curr->left_chid->left_child)
    //     set_name(inserted, curr->left_child->right_sib->value_string)

    //   // Compound statement
    //   compound_n = formal_param_n->right_sib

    //   // Check if left child is VDL
    //   if compound_n->left_child == VDL:
    //     vdl_n = compound_n->left_child

    //     type_specifier type = vdl_n->left_child->left_child

    //     for (curr = vdl_n->left_child->right_sib; curr->right_sib != NULL; curr = curr->right_sib):
    //       inserted = insert_node_into_symtable(symtab, "VD")
    //       set_node_type(inserted, var)
    //       set_type(inserted, type)
    //       set_name(inserted, curr->left->value_string)

    //   leave_scope()

    //   // See above that n still points to FDL node
    //   // and count equals arg count
    //   set_node_arg_count(n, count)

      

     


    // just plug in things we declare -- DON'T NEED TO CHECK (for now)  
    // declaring two id's is an error -- that we should catch

//  */
// /*
//  * handle_ast_node(ast_node n, symboltable_t)
//  */
// int handle_ast_node(ast_node n, symboltable_t symtab) {

//   assert(n);
//   assert(symtab);

//   /* get node type */
//   switch(n->node_type) {
//     case VAR_N;   // HOLD OFF, NECESSARY?
//       // check if n->left_child->value_string is declared variable symbol
//       // n->left_child->right_sibling will be NULL if simple variable
//       // n->left_child->right_sibling will be not null if an array variable
//       break;

//     case FOR_STMT_N: // HOLD OFF, NECESSARY?
//       // first 3 children are FOR_HEADER_NODES, each will exist, but could be empty
//       // if (has_fourth_child && fourth_child == COMPOUND_STMT_N)
//       //    enter new scope for attached compound statment / expression statement
//       //    note: you can't declare a new variable in a 1-liner EXPRESSION_STMT following a for statement
//       break;

//     case DO_WHILE_N: // HOLD OFF, NECESSARY?
//       // do_while must have a compound statement node as the left_child
//       // but it could be empty and not have any children.
//       // if (n->left_child->leftchild != NULL)
//       //    new scope
//       //    dive into compound statement...
//       break;

//     case WHILE_N: // HOLD OFF, NECESSARY?
//       // if (n->left_child->right_sibling == compound statement)
//       //    new scope
//       //    dive into compound statement
//       break;

//     case IF_STMT_N:  // HOLD OFF, NECESSARY?
//       // new scope ?
//       break;

//     case IF_ELSE_STMT_N: // HOLD OFF, NECESSARY?
//       // new scope ?
//       break;

//     case COMPOUND_STMT_N:
//       // new scope
//       //    add children to new scope
//       break;

//     case FORMAL_PARAM_N:
//     case FORMAL_PARAM_ARR_N:
//     case FUNC_DECLARATION_N:
//       // add function declaration symbol to leaf (global)
//       // n->left_child->left_child is type_N
//       // n->left_child->right_sibling is ID_N
//       //
//       // enter new scope
//       // if (n->left_child->right_sibling->right_sibling != VOID_N), then it
//       //    is a FORMAL_PARAM_N with children as FORMAL_PARAMETER / FORMAL_PARAMETER_ARR list of arguments
//       //    add these arguments to the new scope
//       // get (n->left_child->right_sibling->right_sibling->right_sibling)
//       // if ^^^ is a compound statement with children, go to FIRST CHILD
//       // and start add variables to new scope there.
//       //
//       // we need to skip the Compound statement because we've already
//       // entered a new scope.
//       break;

//     case VAR_DECL_N:  
//     case VAR_DECLARATION_N:
//       // type_specifier t = get_type(n->left_child)
//       // for each VAR_DECL_N child, add to current scope
//       //    n->left_child is a type node
//       //    ast_node c = n->left_child->right_sibling
//       //    while (c != NULL)
//       //      if(c->left_child->right_sibling != NULL && c->left_child->right_sibling == INT_LITERAL_N)
//       //         add c as an array variable INT_TS
//       //      else
//       //          add c as a regular variable 
//       break;

//     default:
//       /* These aren't the symbols you're looking for. */
//       break;
//   }

//   // done?
//   return 0?
// }