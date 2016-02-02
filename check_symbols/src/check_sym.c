/* check_sym.c
 * Functions for the checking the symbol table
 *
 * Students: Yondon Fu and Matt McFarland - Delights (CS57 16W)
 */

#include <stdio.h>
#include <assert.h>
#include "types.h"
#include "ast.h"
#include "symtab.h"
#include "check_sym.h"

void set_type(ast_node root) {
	if (!root) 
		return;

	for (ast_node child = root->left_child; child != NULL; child = child->right_sibling)
		set_type(child);

	switch(root->node_type) {
		case VAR_DECL_N:
			// for any variables declared with initialization, make sure that initializing expression has matching type
			break;
		case FUNC_DECLARATION_N:
			// ?

			break;
		case EXPRESSION_N:
			// call set_type on chidren
			// if r-value ... 
			// if variable expression ...
			// assign this node's type based on types of children (should match)

			break;

		case VAR_N: /* could be a single variable instance or array */

			printf("examining node %s with name %s\n", NODE_NAME(root->node_type),root->left_child->value_string);

			char * sym_name = root->left_child->value_string;
			symnode_t * sym_n = find_symnode(root->scope_table, sym_name);

			/* find symbol */
			if (!sym_n) {
				fprintf(stderr,"Undeclared variable: \'%s\'\n", sym_name);
				exit(1);
			} 
			/* check symbol type */
			else if (sym_n->sym_type != VAR_SYM) {
				fprintf(stderr,"Declared symbol \'%s\' isn't a variable\n", sym_name);
				exit(1);
			}

			printf("found symnode\n");

			/* accessing array variable */
			if (root->left_child->right_sibling != NULL) {

				printf("root is an array\n");

				if (sym_n->s.v.modifier != ARRAY_DT) {
					fprintf(stderr,"symbol \'%s\' is not an array\n",sym_name);
					exit(1);
				}

				//set_type(root->left_child->right_sibling);

				/* array index should be an int */
				if (root->left_child->right_sibling->type != INT_TS || 		
					root->left_child->right_sibling->mod  != SINGLE_DT) {
					fprintf(stderr,"Array index for symbol \'%s\' is not a single integer\n", sym_name);
					exit(1);
				}
			} 

			/* accessing single variable */
			else {

				printf("root is a single variable\n");

				if (sym_n->s.v.modifier != SINGLE_DT) {
					fprintf(stderr,"symbol \'%s\' is not an single variable\n", sym_name);
					exit(1);
				}
			}

			/* set ast_node's type and modifier */
			root->type 	= sym_n->s.v.type;
			root->mod 	= SINGLE_DT;
			break;

		case CALL_N:
			// make sure function declaration is a valid symbol
			// make sure correct argument # and types are given
			// make sure returned type matches expression
			break;

		default:
			break;

	}

	return;
} 

/*
 * starting at lowest scope, searches for symbol with name
 *
 * returns NULL if not found
 */
symnode_t * find_symnode(symhashtable_t * hashtable, char * name) {

	assert(hashtable);

	symnode_t * found = lookup_symhashtable(hashtable, name, NOHASHSLOT);
	while (!found && hashtable->parent != NULL) {
		hashtable = hashtable->parent; 	// go up scope to search for symbol
		found = lookup_symhashtable(hashtable, name, NOHASHSLOT);
	}

	return found;
} 