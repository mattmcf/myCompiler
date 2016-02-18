#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "symtab.h"
#include "y86_code_gen.h"
#include "types.h"

/**
 * http://stackoverflow.com/questions/1021935/assembly-y86-stack-and-call-pushl-popl-and-ret-instructions
 * Pushing and popping from stack for function calls
 */

void print_code(quad * to_translate, FILE * ys_file_ptr) {
	switch (to_translate->op) {
		case ADD_Q:
			{
				char * t1 = handle_quad_arg(to_translate->args[1]);
				char * t2 = handle_quad_arg(to_translate->args[2]);
				char * t3 = handle_quad_arg(to_translate->args[0]);
				fprintf(ys_file_ptr, "mrmovl %s(%%ebp), %%eax\n", t1);
				fprintf(ys_file_ptr, "mrmovl %s(%%ebp), %%ebx\n", t2);
				fprintf(ys_file_ptr, "addl %%ebx, %%eax\n");
				fprintf(ys_file_ptr, "rmmovl %%eax, %s(%%ebp)\n", t3);
				break;
			}

		case SUB_Q:
			{
				char * t1 = handle_quad_arg(to_translate->args[1]);
				char * t2 = handle_quad_arg(to_translate->args[2]);
				char * t3 = handle_quad_arg(to_translate->args[0]);
				fprintf(ys_file_ptr, "mrmovl %s(%%ebp), %%eax\n", t1);
				fprintf(ys_file_ptr, "mrmovl %s(%%ebp), %%ebx\n", t2);
				fprintf(ys_file_ptr, "subl %%ebx, %%eax\n");
				fprintf(ys_file_ptr, "rmmovl %%eax, %s(%%ebp)\n", t3);
				break;
			}

		case MUL_Q:
			{
				char * t1 = handle_quad_arg(to_translate->args[1]);
				char * t2 = handle_quad_arg(to_translate->args[2]);
				char * t3 = handle_quad_arg(to_translate->args[0]);
				fprintf(ys_file_ptr, "mrmovl %s(%%ebp), %%eax\n", t1);
				fprintf(ys_file_ptr, "mrmovl %s(%%ebp), %%ebx\n", t2);
				fprintf(ys_file_ptr, "mull %%ebx, %%eax\n");
				fprintf(ys_file_ptr, "rmmovl %%eax, %s(%%ebp)\n", t3);
				break;
			}

		case DIV_Q:
			{
				char * t1 = handle_quad_arg(to_translate->args[1]);
				char * t2 = handle_quad_arg(to_translate->args[2]);
				char * t3 = handle_quad_arg(to_translate->args[0]);
				fprintf(ys_file_ptr, "mrmovl %s(%%ebp), %%eax\n", t1);
				fprintf(ys_file_ptr, "mrmovl %s(%%ebp), %%ebx\n", t2);
				fprintf(ys_file_ptr, "divl %%ebx, %%eax\n");
				fprintf(ys_file_ptr, "rmmovl %%eax, %s(%%ebp)\n", t3);
				break;
			}

		case MOD_Q:
			{
				char * t1 = handle_quad_arg(to_translate->args[1]);
				char * t2 = handle_quad_arg(to_translate->args[2]);
				char * t3 = handle_quad_arg(to_translate->args[0]);
				fprintf(ys_file_ptr, "mrmovl %s(%%ebp), %%eax\n", t1);
				fprintf(ys_file_ptr, "mrmovl %s(%%ebp), %%ebx\n", t2);
				fprintf(ys_file_ptr, "modl %%ebx, %%eax\n");
				fprintf(ys_file_ptr, "rmmovl %%eax, %s(%%ebp)\n", t3);
				break;
			}

		case INC_Q:
			break;

		case DEC_Q:
			break;

		case NOT_Q:
			break;

		case NEG_Q:
			break;

		case ASSIGN_Q:
			break;

		case LT_Q:
			break;

		case GT_Q:
			break;

		case LTE_Q:
			break;

		case NE_Q:
			break;

		case EQ_Q:
			break;

		case IFFALSE_Q:
			break;

		case GOTO_Q:
			break;

		case PRINT_Q:
			break;

		case READ_Q:
			break;

		case PROLOG_Q:
			fprintf(ys_file_ptr, "rrmovl %%esp, %%ebp\n");
			// ...
			break;

		case EPILOG_Q:
			break;

		case PRECALL_Q:
			fprintf(ys_file_ptr, "pushl %%ebp\n");

			break;

		case POSTRET_Q:
			break;

		case PARAM_Q:
			{
				char * t1 = handle_quad_arg(to_translate[0]);

				fprintf(ys_file_ptr, "mrmovl %s(%%ebp), %%eax\n", t1);
				fprintf(ys_file_ptr, "pushl %%eax\n");

				break;
			}

		case RET_Q:
			break;

		case STRING_Q:
			break;

		case LABEL_Q:
			break;

		default:
			break;
	}
}

char * handle_quad_arg(quad_arg * arg) {
	char * to_return;
	switch (arg->type) {
		case INT_LITERAL_Q_ARG:
			{
				int len = floor(log10(abs(INT_MAX))) + 2;
				char int_str[len];
				sprintf(int_str, "%d", arg->int_literal);
				to_return = strdup(int_str);
				break;
			}

		case TEMP_VAR_Q_ARG:
			{
				int fp_offset = ((symnode_t *) arg->temp->temp_symnode)->s.v.offset_of_frame_pointer;

				int len = floor(log10(abs(INT_MAX))) + 2;
				char int_str[len];
				sprintf(int_str, "%d", fp_offset);
				to_return = strdup(int_str);
				break;
			}

		case SYMBOL_VAR_Q_ARG:
			{
				int fp_offset = arg->symnode->s.v.offset_of_frame_pointer;

				int len = floor(log10(abs(INT_MAX))) + 2;
				char int_str[len];
				sprintf(int_str, "%d", fp_offset);
				to_return = strdup(int_str);
				break;
			}

		case SYMBOL_ARR_Q_ARG:
			{
				int offset = arg->symnode->s.v.offset_of_frame_pointer;

				if (arg->int_literal >= 0) {
					if (arg->symnode->s.v.type == INT_TS) {
						offset += arg->int_literal * sizeof(int);
					} else {
						printf("Array elements not integers, unknown type.\n");
					}
				} 

				int len = floor(log10(abs(INT_MAX))) + 2;
				char int_str[len];
				sprintf(int_str, "%d", offset);
				to_return = strdup(int_str);
				break;
			}

		case LABEL_Q_ARG:
			to_return = arg->label;
			break;

		case RETURN_Q_ARG:
			to_return = strdup("%eax");
			break;

		case NULL_ARG:
			break;

		default:
			break;
	}

	return to_return;
}

/*
 * before generating code, set all your frame pointer offsets for variables
 *
 * call this on root with `set_fp_offsets(symtab,0);`
 *
 * returns the address where the stack pointer should be set before execution
 */
void * set_variable_memory_locations(symboltable_t * symtab) {
	if (!symtab) {
		fprintf(stderr,"cannot set memory locations when symboltable is null!\n");
		return NULL;
	}

	symhashtable_t * global_scope = symtab->root;

	/* set all global variable symbols */
	int globals_size = 0;
	symnode_t * sym;
	for (int i = 0; i < global_scope->size; i++) {
		if (global_scope->table[i] != NULL) {
			sym = global_scope->table[i];
			while (sym != NULL) {

				/* for all global variables */
				if (sym->sym_type == VAR_SYM) {
					if (sym->s.v.modifier == SINGLE_DT) {

						/* put single variable top of global list */
						sym->s.v.offset_of_frame_pointer = globals_size;
						sym->s.v.specie = GLOBAL_VAR;						
						globals_size += TYPE_SIZE(sym->s.v.type);						
					} else {

						/* put array on top of global list */
						sym->s.v.offset_of_frame_pointer = globals_size;
						sym->s.v.specie = GLOBAL_VAR;
						int bytes = sym->origin->left_child->right_sibling->value_int * TYPE_SIZE(sym->s.v.type);
						globals_size += bytes;
					}

				}
				sym = sym->next; 	// get next global variable				
			}
		}
	}

	/* for each function scope, set parameters, locals and temps locations in reference to the FP */
	for (symhashtable_t * child = symtab->root->child; child != NULL; child = child->rightsib) {
		set_fp_offsets(child, 0, TYPE_SIZE(INT_TS));
	}

	void * stack_start = (void *) ((void *)STK_TOP - (void *)globals_size);
	return stack_start;
}

/*
 * called ONCE on the function scope table and then it explores down and sets variables
 */
void set_fp_offsets(symhashtable_t * symhash, int local_bytes, int param_bytes) {
	if (!symhash)
		return;

	symnode_t * sym;
	for (int i = 0; i < symhash->size; i++) {
		if (symhash->table[i] != NULL) {

			sym = symhash->table[i];
			while (sym != NULL) {

				/* for all variables in scope */
				if (sym->sym_type == VAR_SYM) {

					// get specie
					switch (sym->s.v.specie) {
						case PARAMETER_VAR:
							/* this was done during the creation of the parameter symbols */
							// sym->s.v.offset_of_frame_pointer = param_bytes;
							// param_bytes += TYPE_SIZE(sym->s.v.type);
							break;

						/* treat locals and locals the same */
						case TEMP_VAR:
						case LOCAL_VAR:

							if (sym->s.v.modifier == SINGLE_DT) {

								/* put single variable on stack*/
								local_bytes -= TYPE_SIZE(sym->s.v.type);
								sym->s.v.offset_of_frame_pointer = local_bytes;														
							} else {

								/* put array on stack */
								local_bytes -= sym->origin->left_child->right_sibling->value_int * TYPE_SIZE(sym->s.v.type);
								sym->s.v.offset_of_frame_pointer = local_bytes;
							}	
							break;

						default:
							break;
					}
				}
				sym = sym->next; 	// get next global variable				
			}
		}
	}

	for (symhashtable_t * sub_scope = symhash->child; sub_scope != NULL; sub_scope = sub_scope->rightsib)
		set_fp_offsets(sub_scope, local_bytes, param_bytes);

	return;
}

/*
 * if at the end of the symbolhashtable, returns NULL
 */
// symnode_t * get_next_symbol(symhashtable_t * symhash, int last_slot, symnode_t * last_seen) {
// 	// if(!symhash)
// 	// 	return NULL;

// 	// symnode_t * next_node = NULL;

// 	// // FIRST CASE -- IS LAST SEEN NOT NULL?
// 	// if (last_seen != NULL) {
// 	// 	if (last_seen->next != NULL) { 			// not done with linked list
// 	// 		next_node = last_seen->next;
// 	// 	} else { 								// get to next bucket
// 	// 		for (int i = slot)
// 	// 	}
// 	// }

// 	return NULL;
// }
