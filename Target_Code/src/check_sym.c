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

extern int type_error_count;

/*
 * starting at lowest scope, searches for symbol with name
 *
 * returns NULL if not found
 */
symnode_t * find_symnode(symhashtable_t * hashtable, char * name);

/*
 * check argument types -> expected number of child of type and modifier
 *
 * returns 0 if no error
 * returns 1 if error
 */
int check_op_arg_types(ast_node op_node, int child_count, type_specifier_t type, modifier_t mod);

/*
 * check VAR_N for correct types
 *
 * returns 1 if errors occurs
 * else returns 0 if all good
 */
int check_var_node(ast_node root);

/* 
 * check var_declaration node for initialization types
 *
 * returns 0 if all checks out
 * returns 1 if type mismatch
 */
int check_var_declaration(ast_node root);

/*
 * Checks a function declaration node for appropriate argument types and
 * for the right return statements.
 * 
 * Note: this type_err() inside the function whenever an incorrect return 
 * is found in the find return statement.
 *
 * returns 1 if errors occurs
 * else returns 0 if all good
 */
int check_fdl_node(ast_node root);

/*
 * Checks a function call for appropriate arguments and sets return type
 *
 * Returns 1 if errors occur
 * else returns 0
 */
int check_call(ast_node root);

int check_sizeof(ast_node root);

/*
 * find_return -- runs down the children of a function's compound statement, searching for
 * return statements. Returns 0 if expected type matches up. Returns 1 if there's a mismatch.
 * Recursively calls itself on node children. Also points a RETURN_N node to it's parent function.
 */
int find_return(type_specifier_t return_type, modifier_t mod_type, ast_node function_header, int * return_flag, ast_node root);

/*
 * Recursive function that implements all top-down type checking
 */
void set_type(ast_node root) {

	if (!root) 
		return;

	/* top-down means setting children types and then synthesizing root type before returning */
	for (ast_node child = root->left_child; child != NULL; child = child->right_sibling)
		set_type(child);

	switch(root->node_type) {

		/* 
		 * Handle Base cases
		 */
		case TYPEINT_N:
			root->type 	= INT_TS;
			root->mod 	= SINGLE_DT;
			break;

		case INT_LITERAL_N:
			root->type 	= INT_TS;
			root->mod 	= SINGLE_DT;
			break;

		case VOID_N:
			root->type 	= VOID_TS;
			root->mod 	= SINGLE_DT;
			break;

		case TYPE_SPEC_N:
			root->type 	= root->left_child->type;
			root->mod 	= root->left_child->mod;
			break;

		case EXPRESSION_N:
			/* check if expression is variable assignment or r-value */
			if (root->left_child->right_sibling != NULL) {

				/* var = expr production */
				ast_node var = root->left_child;
				ast_node expr = root->left_child->right_sibling;

				/* can only do assignment between like types and for single data types */
				if ( (var->type == expr->type) && (var->mod == SINGLE_DT) && (expr->mod == SINGLE_DT) ) {
					root->type 	= var->type;
					root->mod 	= expr->mod;
				} else {
					type_err(root);
					root->type 	= NULL_TS;
					root->mod 	= NULL_DT;
				}

			} else {

				/* r_value production */
				if (root->left_child->type != NULL_TS && root->left_child->mod != NULL_DT) {
					root->type 	= root->left_child->type;
					root->mod 	= root->left_child->mod;					
				} else { 
					type_err(root);
					root->type 	= NULL_TS;
					root->mod 	= NULL_DT;
				}
			}
			break;

		case OP_ASSIGN_N:
			{
				ast_node var = root->left_child;
				ast_node expr = root->left_child->right_sibling;		
				if ( (var->type == expr->type) && ((var->mod == SINGLE_DT) && (expr->mod == SINGLE_DT)) ) {
					root->type 	= var->type;
					root->mod 	= expr->mod;
				} else {
					type_err(root);
					root->type 	= NULL_TS;
					root->mod 	= NULL_DT;
				}
			}
			break;
	
		/* 
		 * Handle r-value operand nodes -> both arguments must be same type 
		 */
		case OP_PLUS_N:
		case OP_MINUS_N:
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
				type_err(root);
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
		case OP_NEG_N:
		case OP_NOT_N:
		case OP_PRE_INC_N:
		case OP_PRE_DEC_N:
		case OP_POST_INC_N:
		case OP_POST_DEC_N:
			if (check_op_arg_types(root, 1, INT_TS, SINGLE_DT)) {

				fprintf(stderr,"mismatching type arguments for operation %s\n", NODE_NAME(root->node_type));
				type_err(root);
				root->type 	= NULL_TS;
				root->mod 	= NULL_DT;

			} else {

				/* no error -- expecting single ints only, so not scaled for more types */
				root->type 	= INT_TS;
				root->mod 	= SINGLE_DT;
			}
			break;

		/*
		 * Handle Variable / Function Declaration and initialization cases
		 */
		case VAR_DECLARATION_N:
			if (root->left_child->type == VOID_TS) {
				type_err(root);
				fprintf(stderr, "cannot have void variables\n");
			} 

			break;

		case VAR_DECL_N:
			if (check_var_declaration(root)) {
				type_err(root);
				fprintf(stderr,"variable %s has improper type assignment\n",root->left_child->value_string);
			}
			break;

		case FUNC_DECLARATION_N:
			if (check_fdl_node(root)) {
				type_err(root);
				fprintf(stderr,"function declaration for \'%s\' contains errors in body\n", root->left_child->right_sibling->value_string);
			}		
			break;

		/*
		 * Handle Variable Call
		 */
		case VAR_N: /* could be a single variable instance or array */

			if (check_var_node(root)) {
				/* error occurred so set error values */
				type_err(root);
				root->type 	= NULL_TS;
				root->mod 	= NULL_DT;
			}

			break;

		/* 
		 * Handle Function Call
		 */
		case CALL_N:
			if (check_call(root)) {
				/* error in arguments or unrecognized function */
				type_err(root);
				root->type = NULL_TS;
				root->mod = NULL_DT;
			}		
			break;

		case SIZEOF_N:
			if (check_sizeof(root)) {
				type_err(root);
				root->type = NULL_TS;
				root->mod = NULL_DT;
			}
			break;

		case RETURN_N:
			root->type 	= root->left_child->type;
			root->mod 	= root->left_child->mod;
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

	root->type 	= sym_n->s.v.type;
	root->mod 	= sym_n->s.v.modifier;

	if (root->mod == ARRAY_DT && root->left_child->right_sibling != NULL) {

		/* array index should be an int */
		if (root->left_child->right_sibling->type != INT_TS || 		
			root->left_child->right_sibling->mod  != SINGLE_DT) {
			fprintf(stderr,"Array index for symbol \'%s\' is not a single integer\n", sym_name);
			return 1;
		} else {

			/* ACCESSING ARRAY LIKE IT'S AN ARRAY, SO SET DATA TYPE TO SINGLE! */
			root->mod = SINGLE_DT;
		}	
	} else if (root->mod != ARRAY_DT && root->left_child->right_sibling != NULL) {

		fprintf(stderr,"symbol \'%s\' is not an array\n", sym_name);
		return 1;
	}

	return 0;
}

/* 
 * returns 0 if all checks out
 * returns 1 if type mismatch
 */
int check_var_declaration(ast_node root) {
	assert(root);

	symnode_t * sym = find_symnode(root->scope_table, root->left_child->value_string);
	if (!sym)
		return 1;

	type_specifier_t t	= sym->s.v.type;
	modifier_t 	m	 	= sym->s.v.modifier;

	if (root->left_child->right_sibling != NULL && root->left_child->right_sibling->type != INT_TS ) {

		/* check initialization assignment */
		if (root->left_child->right_sibling->type != t || root->left_child->right_sibling->mod != m) {
			return 1;
		}
	}

	return 0;
}

/*
 * returns 1 if errors occurs
 * else returns 0 if all good
 */
int check_fdl_node(ast_node root) {
	assert(root);

	type_specifier_t ret_type 	= root->left_child->type;
	modifier_t mod 				= root->left_child->mod;

	int found_return = 0;
	if (find_return(ret_type,mod, root, &found_return, root->left_child->right_sibling->right_sibling->right_sibling)) {

		/* found return statements with non matching return types */
		return 1;
	}

	/* if returning void without a return statement, add one */
	if (found_return == 0) {

		if (ret_type == VOID_TS) {
			/* add a void return statement */

			/* get compound statement node */
			ast_node new_return;
			ast_node stmt = root->left_child->right_sibling->right_sibling->right_sibling;
			
			if (stmt->left_child == NULL) {

				/* add as first child of empty compound statement list */
				stmt->left_child = create_ast_node(RETURN_N);
				assert(stmt->left_child);
				new_return = stmt->left_child;
			} else {

				/* get to end of compound statement child list */
				ast_node add_return_at;
				for (add_return_at = stmt->left_child; 
						add_return_at->right_sibling != NULL;
						add_return_at = add_return_at->right_sibling);

				add_return_at->right_sibling = create_ast_node(RETURN_N);
				assert(add_return_at->right_sibling);	
				new_return = add_return_at->right_sibling;
			}

			/* add the void type to new return node */
			new_return->left_child = create_ast_node(VOID_N);
			assert(new_return->left_child);

			/* lazily set types and modifier fields in new return node */
			set_type(new_return); 		
			new_return->parent_function = root;	

		} else {
			fprintf(stderr,"function %s does not have a return statement\n",root->left_child->right_sibling->value_string);
			return 1;			
		}
	
	} 

	return 0;
}


int find_return(type_specifier_t return_type, modifier_t mod_type, ast_node function_header, int * return_flag, ast_node root) {
	if (!root)
		return 0;

	if (root->node_type == RETURN_N){
		(*return_flag)++; 	// found a return statement

		if (root->type != return_type || root->mod != mod_type) {
			type_err(root);
			fprintf(stderr,"return statement on line %d doesn't have correct type (has %s type and %s mod, expecting %s and %s)\n", 
				root->line_number, TYPE_NAME(root->type), MODIFIER_NAME(root->mod), TYPE_NAME(return_type), MODIFIER_NAME(mod_type));
			return 1;			
		} else {

			/* return statement checks out -- point to parent function */
			root->parent_function = function_header;
			return 0;
		}
	}

	int rc = 0;
	for (ast_node child = root->left_child; child != NULL; child = child->right_sibling) {
		rc += find_return(return_type, mod_type, function_header, return_flag, child);
	}
		
	return rc;
}

int check_call(ast_node root) {
	// Look up function in symtab using function identifier

	/* get to global scope */
	symhashtable_t * global_scope;
	for (global_scope = root->scope_table; global_scope->parent != NULL; global_scope = global_scope->parent);

	symnode_t *func = lookup_symhashtable(global_scope, root->left_child->value_string, NOHASHSLOT);

	if (func == NULL) {
		fprintf(stderr, "Undeclared function: \'%s\'\n", root->left_child->value_string);
		return 1;
	} else if (func->sym_type != FUNC_SYM) {
		fprintf(stderr, "Declared symbol \'%s\' isn't a function\n", root->left_child->value_string);
		return 1;
	}

	var_symbol * func_args = func->s.f.arg_arr;
	int func_arg_count = func->s.f.arg_count;

	// Access arg list
	if (root->left_child->right_sibling != NULL) {
		int arg_count = 0;

		ast_node arg;
		for (arg = root->left_child->right_sibling->left_child; arg != NULL; arg = arg->right_sibling) {
			arg_count++;
			if (arg_count > func_arg_count) {
				fprintf(stderr, "Mismatched arg count for function %s. Expected %d, got %d\n", 
					func->name, func_arg_count, arg_count);
				return 1;
			}
			// Check if each argument is of the right type and modifier
			if (func_args != NULL && (arg->type != func_args[arg_count-1].type || arg->mod != func_args[arg_count-1].modifier)) {
				fprintf(stderr, "Mismatched arg type for function %s. Expected type %s, got type %s. Expecting modifier %s, got modifier %s.\n", 
					func->name, TYPE_NAME(func_args[arg_count].type), TYPE_NAME(arg->type),
					MODIFIER_NAME(func_args[arg_count].modifier), MODIFIER_NAME(arg->mod));
				return 1;
			}

		}

		if (arg_count != func_arg_count) {
			fprintf(stderr, "Mismatched arg count for function %s. Expected %d, got %d\n", func->name, func_arg_count, arg_count);
			return 1;
		}
	}

	// Set this node's type to function's return type
	root->type = func->s.f.return_type;
	root->mod = SINGLE_DT;

	return 0;
}

int check_sizeof(ast_node root) {
	root->type = INT_TS;
	root->mod = SINGLE_DT;
	return 0;
}

void type_err(ast_node root) {
	assert(root);
	type_error_count++;
	fprintf(stderr, "Type error on line %d of program (node %s)\n", get_line_number(root),NODE_NAME(root->node_type));
}


