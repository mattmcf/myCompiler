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

/*
 * check VAR_N for correct types
 *
 * returns 1 if errors occurs
 * else returns 0 if all good
 */
int check_var_node(ast_node root);

void set_type(ast_node root) {
	if (!root) 
		return;

	/* have children set types before setting this node's type / modifier */
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

		/* 
		 * Handle r-value operand nodes -> both arguments must be same type 
		 */
		case OP_PLUS_N:
		case OP_MINUS_N:
		case OP_NEG_N:
		case OP_TIMES_N:
		case OP_DIVIDE_N:
		case OP_MOD_N:
		case OP_LT_N:
		case OP_GT_N:
		case OP_GTE_N:
		case OP_LTE_N:
		case OP_EQ_N:
		case OP_NE_N:
		case OP_AND_N:
		case OP_OR_N:

			if (check_op_arg_types(root, 2, INT_TS, SINGLE_DT)) {

				fprintf(stderr,"mismatching type arguments for operation %s\n", NODE_NAME(root->node_type));
				root->type 	= NULL_TS;
				root->mod 	= NULL_DT;

			} else {

				/* no error -- expecting single ints only, so not scaled for more types */
				root->type 	= INT_TS;
				root->mod 	= SINGLE_DT;
			}

			break;

		/*
		 * Handle single arguments r-value nodes 
		 */
		case OP_INC_N:
		case OP_DECR_N:

			if (check_op_arg_types(root, 1, INT_TS, SINGLE_DT)) {

				fprintf(stderr,"mismatching type arguments for operation %s\n", NODE_NAME(root->node_type));
				root->type 	= NULL_TS;
				root->mod 	= NULL_DT;

			} else {

				/* no error -- expecting single ints only, so not scaled for more types */
				root->type 	= INT_TS;
				root->mod 	= SINGLE_DT;
			}

			break;

		/*
		 * handle variable call
		 */
		case VAR_N: /* could be a single variable instance or array */

			if (check_var_node(root)) {
				/* error occurred so set error values */
				root->type 	= NULL_TS;
				root->mod 	= NULL_DT;
			}

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

/*
 * check argument types -> expected number of child of type and modifier
 *
 * returns 0 if no error
 * returns 1 if error
 */
int check_op_arg_types(ast_node op_node, int child_count, type_specifier_t type, modifier_t mod) {

	assert(op_node);

	if (child_count < 1)
		return 0;

	/* go through specified number of children, checking for matching type and modifier */
	ast_node child = op_node->left_child;
	for(int i = 0; i < child_count; i++) {
		if (!child)
			return 1;
		else if (child->type != type || child->mod != mod) {
			return 1;
		}

		child = child->right_sibling;
	}

	return 0;
}

/*
 * returns 1 if errors occurs
 * else returns 0 if all good
 */
int check_var_node(ast_node root) {
	assert(root);

	printf("examining node %s with name %s\n", NODE_NAME(root->node_type),root->left_child->value_string);

	char * sym_name = root->left_child->value_string;
	symnode_t * sym_n = find_symnode(root->scope_table, sym_name);

	/* find symbol */
	if (!sym_n) {
		fprintf(stderr,"Undeclared variable: \'%s\'\n", sym_name);
		return 1;
	} 
	/* check symbol type */
	else if (sym_n->sym_type != VAR_SYM) {
		fprintf(stderr,"Declared symbol \'%s\' isn't a variable\n", sym_name);
		return 1;
	}

	printf("found symnode\n");

	/* accessing array variable */
	if (root->left_child->right_sibling != NULL) {

		printf("root is an array\n");

		if (sym_n->s.v.modifier != ARRAY_DT) {
			fprintf(stderr,"symbol \'%s\' is not an array\n",sym_name);
			return 1;
		}

		/* array index should be an int */
		if (root->left_child->right_sibling->type != INT_TS || 		
			root->left_child->right_sibling->mod  != SINGLE_DT) {
			fprintf(stderr,"Array index for symbol \'%s\' is not a single integer\n", sym_name);
			return 1;
		}
	} 

	/* accessing single variable */
	else {

		printf("root is a single variable\n");

		if (sym_n->s.v.modifier != SINGLE_DT) {
			fprintf(stderr,"symbol \'%s\' is not an single variable\n", sym_name);
			return 1;
		}
	}

	/* set ast_node's type and modifier */
	root->type 	= sym_n->s.v.type;
	root->mod 	= SINGLE_DT;

	return 0;
}