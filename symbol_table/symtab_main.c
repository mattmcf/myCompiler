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

    printf("******PRETTY PRINTING SYMBOLTABLE******\n");

    // Done filling the symtab
    print_symtab(symtab);
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

    printf("handling function declaration node\n");

    // handle function node declaration and skip to first child of function compound statement
    for (ast_node child = handle_func_decl_node(root,symtab); child != NULL; 
          child = child->right_sibling) {
      traverse_ast_tree(child, symtab);
    }
      
    break;

  case VAR_DECLARATION_N:

    printf("handling variable declaration line node\n");

    handle_var_decl_line_node(root,symtab);
    // don't traverse these children
    // can return now because parent call will move onto sibling
    break;

  case COMPOUND_STMT_N:
    if (root->left_child != NULL)
      enter_scope(symtab, root->left_child, "BLOCK");

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
}


/*
 * handle_func_decl_node(ast_node * fdl, symboltable_t * t);
 */
ast_node handle_func_decl_node(ast_node fdl, symboltable_t * symtab) {

  assert(fdl);
  assert(symtab);

  // printf("Got to handle fdl\n");


  // Insert symnode for function into leaf symhashtable
  symnode_t *fdl_node = insert_into_symboltable(symtab, fdl->left_child->right_sibling->value_string);

  if (fdl_node == NULL) {
    /* duplicate symbol in scope */
    fprintf(stderr, "error: duplicate symbol \'%s\' found. Please fix before continuing.\n", fdl->left_child->right_sibling->value_string);
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
  arg = arg->left_child;

  int arg_count = 0;
  var_symbol * arg_arr = NULL;

  /* collect all argument parameters */
  if (arg->node_type != VOID_N) {
    printf("Parameters for function are not void\n");

    int arg_arr_size = 5;     /* magic number */
    arg_arr = (var_symbol *)calloc(arg_arr_size, sizeof(var_symbol));
    assert(arg_arr);
   
    type_specifier_t type;
    modifier_t mod;
    char * name;

    while (arg != NULL) {

      printf("arg node type is %s\n",NODE_NAME(arg->left_child->node_type));

      /* make a variable for each argument */
      type = get_datatype(arg->left_child->left_child);
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

  printf("finished collecting arguments\n");

  /* add function symbol to current (global) scope */

  // Set fields for func node in current (global) scope
  set_node_func(fdl_node, id, return_type, arg_count, arg_arr);

  /* get CS node */
  ast_node compound_stmt = fdl->left_child->right_sibling->right_sibling->right_sibling;
  ast_node next_node = NULL;

  if (compound_stmt->left_child != NULL) {

    printf("seeing a function declaration compound statement\n");

    /* enter new scope -- skipping CS node */
    enter_scope(symtab, compound_stmt->left_child, fdl_node->name);

    printf("entered a new scope\n");

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
    }

    /* return next node to start traverse on */
    next_node = compound_stmt->left_child;
  } 
  
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
}
