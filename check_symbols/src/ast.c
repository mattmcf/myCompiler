/* ast.c
 *
 * Written by THC for CS 57; massaged by SWS
 *
 * Students: Yondon Fu and Matt McFarland - Delights (CS57 16W)
 *
 * Relies on an enum and a struct for
 * various node types that appear in ast.h.  You should modify the
 * enum and struct as appropriate.
 *
 * This file contains functions to create a node and to print out an
 * abstract syntax tree, for debugging.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "ast.h"
#include "symtab.h"
#include "types.h"

extern int yylineno;

/* Create a node with a given token type and return a pointer to the
   node. */
ast_node create_ast_node(ast_node_type node_type) {
  ast_node new_node = calloc(1,sizeof(struct ast_node_struct));  // for zeros
  if (!new_node) {
    fprintf(stderr,"error creating node of type %s -- ran out of memory.\n", NODE_NAME(node_type));
    exit(1);
  }
  new_node->node_type = node_type;
  new_node->line_number = yylineno;

  return new_node;
}

int get_line_number(ast_node root) {
  assert(root);
  ast_node child;
  int to_return;

  if (root->left_child != NULL) {
    /* go to left most child */
    for (child = root->left_child; child->left_child != NULL; child = child->left_child)
      ;
    to_return = child->line_number;
  } else {
    /* no child, so just return root's line number */
    to_return = root->line_number;
  }
  return to_return;
}

/* Print the contents of a subtree of an abstract syntax tree, given
   the root of the subtree and the depth of the subtree root. */
void print_ast(ast_node root, int depth) {

  /* Print two spaces for every level of depth. */
  int i;
  for (i = 0; i < depth; i++)
    printf("  ");

  if (root == NULL) {
    printf("NULL NODE\n");
    return;
  }

  /* Print the node type. */
  printf("%s, ", NODE_NAME(root->node_type));

  /* Print attributes specific to node types. */
  switch (root->node_type) {
  case ID_N:			/* print the id */
    printf("%s", root->value_string);
    break;

  case INT_LITERAL_N:		/* print the int literal */
    printf("%d", root->value_int);
    break;

  case STRING_N:
    printf("%s", root->value_string);
    break;

  case RETURN_N:
    if (root->parent_function != NULL) {
      printf("returns to %s", root->parent_function->left_child->right_sibling->value_string);
    }
      
  default:
    break;
  }

  if (root->type != NULL_TS || root->mod != NULL_DT)
    printf(" -- type: %s, modifier: %s ", TYPE_NAME(root->type), MODIFIER_NAME(root->mod));

  symhashtable_t * hashtable = (symhashtable_t *)root->scope_table;
  if (hashtable != NULL)
    printf(" (scope: %d-%d %s)", hashtable->level, hashtable->sibno, hashtable->name);

  printf(" [Line Number: %d]", get_line_number(root));
  printf("\n");
  

  /* Recurse on each child of the subtree root, with a depth one
     greater than the root's depth. */
  ast_node child;
  for (child = root->left_child; child != NULL; child = child->right_sibling)
    print_ast(child, depth + 1);
}  
