#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "symtab.h"
#include "y86_code_gen.h"
#include "types.h"

#define MAX_ARG_LEN 	50
#define MAX_HEX_ADDRESS_LEN 11 	// 0x????????'\n' is 11 characters for a string (with null terminator)
#define STK_TOP 0x0000FFFF


extern symboltable_t * symtab; 	// for global lookups of symbols
extern quad_arr * quad_list;

#define DSTR_reg 0x00FFFE10 		// DISPLAY STRING DATA REGISTER
#define DHXR_reg 0x00FFFE14			// DISPLAY HEX REGISTER
#define KSTR_reg 0x00FFFE18 		// KEYBOARD STRING REGISTER (BLOCKING) -- SEE DOCUMENTATION
#define KHXR_reg 0x00FFFE1C 		// KEYBOARD HEX REGISTER (BLOCKING)
#define KBDR_reg 0x00FFFE04 		// KEYBAORD DATA REGISTER 

/**
 * http://stackoverflow.com/questions/1021935/assembly-y86-stack-and-call-pushl-popl-and-ret-instructions
 * Pushing and popping from stack for function calls
 *
 * http://web.cse.ohio-state.edu/~reeves/CSE2421sp13/PracticeProblemsY86.pdf -- more subroutine examples
 */

/*
 * creates ys file from global quad_list
 */
int create_ys(char * file_name) {
	if (!file_name) {
		fprintf(stderr,"cannot create .ys file because title string is null\n");
		return 1;
	}

	int title_len = strlen(file_name + 4);
	char title_str[title_len];
	title_str[0] = '\0';

	strcat(title_str, file_name);
	strcat(title_str, ".ys");
	FILE * ys_fp = fopen(title_str,"w");
	if (!ys_fp) {
		fprintf(stderr,"could not create .ys file %s -- aborting\n",file_name);
		return 1;
	}

	/* stack and base pointer initialization */

	int stk_start = set_variable_memory_locations(symtab);
	printf("stack starks at %x\n",stk_start);
	fprintf(ys_fp,".pos 0\n");	
	print_nop_comment(ys_fp, "initialization", -1);
	fprintf(ys_fp,"\tirmovl 0x%x, %%esp\n",stk_start);
	fprintf(ys_fp,"\trrmovl %%esp, %%ebp\n");
	fprintf(ys_fp,"\tirmovl $4, %%eax\n");
	fprintf(ys_fp,"\tsubl %%eax, %%esp\n");

	fprintf(ys_fp, "\tcall main\n");
	fprintf(ys_fp, "\thalt\n");

	/* translate quad list */

	for (int i = 0; i < quad_list->count; i++) {
		printf("looking at quad %d\n",i);
		print_code(quad_list->arr[i], ys_fp);
	}

	/* wrap up */

	// print_nop_comment(ys_fp, "done", -1);
	// fprintf(ys_fp,"\thalt\n\n");	
	fclose(ys_fp);

	printf("\n----- PRINTED YS FILE %s ----- \n",file_name);
	return 0;
}

/*
 * print a quad
 */
void print_code(quad * to_translate, FILE * ys_file_ptr) {
	switch (to_translate->op) {
		case ADD_Q:
			{
				char * t1 = handle_quad_arg(to_translate->args[1]);
				char * t2 = handle_quad_arg(to_translate->args[2]);
				char * t3 = handle_quad_arg(to_translate->args[0]);
				fprintf(ys_file_ptr, "\t%s %s, %%eax\n", get_move_type(to_translate->args[1]),t1);
				fprintf(ys_file_ptr, "\t%s %s, %%ebx\n", get_move_type(to_translate->args[2]),t2);
				fprintf(ys_file_ptr, "\taddl %%ebx, %%eax\n");
				fprintf(ys_file_ptr, "\trmmovl %%eax, %s\n", t3);
				break;
			}

		case SUB_Q:
			{
				char * t1 = handle_quad_arg(to_translate->args[1]);
				char * t2 = handle_quad_arg(to_translate->args[2]);
				char * t3 = handle_quad_arg(to_translate->args[0]);
				fprintf(ys_file_ptr, "\t%s %s, %%eax\n", get_move_type(to_translate->args[1]),t1);
				fprintf(ys_file_ptr, "\t%s %s, %%ebx\n", get_move_type(to_translate->args[2]),t2);
				fprintf(ys_file_ptr, "\tsubl %%ebx, %%eax\n");
				fprintf(ys_file_ptr, "\trmmovl %%eax, %s\n", t3);
				break;
			}

		case MUL_Q:
			{
				char * t1 = handle_quad_arg(to_translate->args[1]);
				char * t2 = handle_quad_arg(to_translate->args[2]);
				char * t3 = handle_quad_arg(to_translate->args[0]);
				fprintf(ys_file_ptr, "\t%s %s, %%eax\n", get_move_type(to_translate->args[1]),t1);
				fprintf(ys_file_ptr, "\t%s %s, %%ebx\n", get_move_type(to_translate->args[2]),t2);
				fprintf(ys_file_ptr, "\tmull %%ebx, %%eax\n");
				fprintf(ys_file_ptr, "\trmmovl %%eax, %s\n", t3);
				break;
			}

		case DIV_Q:
			{
				char * t1 = handle_quad_arg(to_translate->args[1]);
				char * t2 = handle_quad_arg(to_translate->args[2]);
				char * t3 = handle_quad_arg(to_translate->args[0]);
				fprintf(ys_file_ptr, "\t%s %s, %%eax\n", get_move_type(to_translate->args[1]),t1);
				fprintf(ys_file_ptr, "\t%s %s, %%ebx\n", get_move_type(to_translate->args[2]),t2);
				fprintf(ys_file_ptr, "\tdivl %%ebx, %%eax\n");
				fprintf(ys_file_ptr, "\trmmovl %%eax, %s\n", t3);
				break;
			}

		case MOD_Q:
			{
				char * t1 = handle_quad_arg(to_translate->args[1]);
				char * t2 = handle_quad_arg(to_translate->args[2]);
				char * t3 = handle_quad_arg(to_translate->args[0]);
				fprintf(ys_file_ptr, "\t%s %s, %%eax\n", get_move_type(to_translate->args[1]),t1);
				fprintf(ys_file_ptr, "\t%s %s, %%ebx\n", get_move_type(to_translate->args[2]),t2);
				fprintf(ys_file_ptr, "\tmodl %%ebx, %%eax\n");
				fprintf(ys_file_ptr, "\trmmovl %%eax, %s\n", t3);
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
			print_nop_comment(ys_file_ptr, "assignment", to_translate->number);
			{
				char * t1 = handle_quad_arg(to_translate->args[1]);
				char * t3 = handle_quad_arg(to_translate->args[0]);
				fprintf(ys_file_ptr, "\t%s %s, %%eax\n", get_move_type(to_translate->args[1]),t1);
				fprintf(ys_file_ptr, "\trmmovl %%eax, %s\n", t3);
				break;
			}		
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
			{
				char * jmp_label = handle_quad_arg(to_translate->args[0]);
				fprintf(ys_file_ptr,"\tjmp %s\n",jmp_label); 
			}
			break;

		case PRINT_Q:
			{
				switch(to_translate->args[0]->type){
					case LABEL_Q_ARG:
						fprintf(ys_file_ptr, "\tirmovl %s, %%eax\n",to_translate->args[0]->label);
						fprintf(ys_file_ptr, "\trmmovl %%eax, 0x%x\n",DSTR_reg);
						break;

					default:
						break;
				}
			}
			break;

		case READ_Q:
			break;

		case PROLOG_Q:
			{
				print_nop_comment(ys_file_ptr, "function prolog", to_translate->number);

				char * t1 = handle_quad_arg(to_translate->args[0]);
				printf("looking for %s in symboltable\n",t1);
				symnode_t * func_sym = find_in_top_symboltable(symtab, t1);

				// error check
				if (!func_sym) {
					fprintf(stderr,"error during code generation: couldn't find function symbol %s\n", t1);
					exit(1);
				}

				fprintf(ys_file_ptr, "%s:\n",t1);
				fprintf(ys_file_ptr, "\tpushl %%ebp\n");			
				fprintf(ys_file_ptr, "\trrmovl %%esp, %%ebp\n"); 		// move esp to ebp
				/* 
				 * --- set esp to bottom of local and temp space --- 
				 * esp should be set to symnode->s.f.stk_offset for function's symbol
				 */
				fprintf(ys_file_ptr, "\tirmovl $%d, %%eax\n",func_sym->s.f.stk_offset);
				fprintf(ys_file_ptr, "\tsubl %%eax, %%esp\n");				
			}
			break;

		case EPILOG_Q:
			{
				print_nop_comment(ys_file_ptr, "function epilog", to_translate->number);

				//char * epilog_label = handle_quad_arg(to_translate->args[0]);
				//fprintf(ys_file_ptr, "%s:\n",epilog_label);
				fprintf(ys_file_ptr, "\trrmovl %%ebp, %%esp\n");
				fprintf(ys_file_ptr, "\tpopl %%ebp\n"); 						// return to old frame pointer
				fprintf(ys_file_ptr, "\tret\n");
			}

			break;

		case PRECALL_Q:
			{
				print_nop_comment(ys_file_ptr, "function precall", to_translate->number);
				char * func_label = handle_quad_arg(to_translate->args[0]); 	// get function label	
				fprintf(ys_file_ptr, "\tcall %s\n",func_label); 								// pushes ret addr on stack				
			}
			break;

		case POSTRET_Q:
			// return value is in %eax
			// pop each argument? or just move esp back to the offset of the function {}		
			{
				print_nop_comment(ys_file_ptr, "post return", to_translate->number);
				char * t1 = handle_quad_arg(to_translate->args[0]); 										// get function string for symboltable lookup
				symnode_t * func_sym = find_in_top_symboltable(symtab, t1);		
				fprintf(ys_file_ptr, "\tirmovl $%d, %%ebx\n",func_sym->s.f.stk_offset); 	// %ebx b/c return lives in %eax
				fprintf(ys_file_ptr, "\tsubl %%ebx, %%esp\n");								
			}
			break;

		case PARAM_Q:

		/* need to handle case when handling an array pointer or just an array element -- PASS_ARR_POINTER from IR_gen.c 
		 * if the int literal is -1 -> pass array head address
		 * if the int literal isn't -1, copy the data at the element to a param and then push it
		 */

			{
				char * t1 = handle_quad_arg(to_translate->args[0]);
				fprintf(ys_file_ptr, "\tmrmovl %s, %%eax\n", t1);
				fprintf(ys_file_ptr, "\tpushl %%eax\n");
				break;
			}

		case RET_Q:
			print_nop_comment(ys_file_ptr, "return statement", to_translate->number);

			// void return
			if (to_translate->args[0] == NULL) {
				fprintf(ys_file_ptr, "\tirmovl $0, %%eax\n"); 	// clear return value for void

			// constant return
			} else if (to_translate->args[0]->type == INT_LITERAL_Q_ARG) {
				fprintf(ys_file_ptr, "\tirmovl $%s, %%eax\n", handle_quad_arg(to_translate->args[0]));

			// variable return
			} else {
				fprintf(ys_file_ptr, "\tmrmovl %s, %%eax\n", handle_quad_arg(to_translate->args[0]));
			}
			break;

		case STRING_Q:
			{
				int len = strlen(to_translate->args[0]->label);
				//fprintf(ys_file_ptr,"\t.align 4\n");
				for (int i = 0; i < len; i++) {
					fprintf(ys_file_ptr,"\t.byte 0x%x\n",to_translate->args[0]->label[i]);
				}
				fprintf(ys_file_ptr,"\t.byte 0x0\n"); 	// null terminator
			}
			break;

		case LABEL_Q:
			{
				char * t1 = handle_quad_arg(to_translate->args[0]); 			// get string for label
				fprintf(ys_file_ptr,"%s:\n",t1);
			}
			break;

		default:
			break;
	}
}

char * get_move_type(quad_arg * src) {
	if (!src)
		return NULL;

	char * mov_str;
	if (src->type == RETURN_Q_ARG) {
		mov_str = strdup("rrmovl");
	} else if (src->type == INT_LITERAL_Q_ARG){
		mov_str = strdup("irmovl");
	} else {
		mov_str = strdup("mrmovl");
	}

	return mov_str;
}

char * handle_quad_arg(quad_arg * arg) {
	char * to_return;
	printf("entering handle quad arg\n");

	switch (arg->type) {
		case INT_LITERAL_Q_ARG:
			printf("int literal arg");
			{
				//int len = floor(log10(abs(INT_MAX))) + 2;
				char int_str[MAX_ARG_LEN];
				sprintf(int_str, "%d", arg->int_literal);
				to_return = strdup(int_str);
				break;
			}

		case TEMP_VAR_Q_ARG:
			printf("temp arg\n");
			{
				int fp_offset = ((symnode_t *) arg->temp->temp_symnode)->s.v.offset_of_frame_pointer;

				int len = floor(log10(abs(INT_MAX))) + 2;
				char int_str[len];
				sprintf(int_str, "%d(%%ebp)", fp_offset);
				to_return = strdup(int_str);
				break;
			}

		case SYMBOL_VAR_Q_ARG:
			printf("symbol variable arg\n");

			if (arg->symnode->s.v.specie == GLOBAL_VAR) {

				/* return absolute address */
				char global_address_str[MAX_HEX_ADDRESS_LEN];
				sprintf(global_address_str, "0x%x",arg->symnode->s.v.offset_of_frame_pointer); 	// global address
				to_return = strdup(global_address_str);	

			} else {

				/* return relative address */
				int fp_offset = arg->symnode->s.v.offset_of_frame_pointer;
				int len = floor(log10(abs(INT_MAX))) + 2;
				char int_str[len];
				sprintf(int_str, "%d(%%ebp)", fp_offset);
				to_return = strdup(int_str);				
			}
			break;

		case SYMBOL_ARR_Q_ARG:
			printf("symbol array arg\n");

			if (arg->symnode->s.v.specie == GLOBAL_VAR) {

				/* return absolute address within global array */
				int offset = arg->symnode->s.v.offset_of_frame_pointer;
				char global_address_str[MAX_HEX_ADDRESS_LEN];

				if (arg->int_literal >= 0) {
					if (arg->symnode->s.v.type == INT_TS) {
						offset += arg->int_literal * TYPE_SIZE(INT_TS);
					} else {
						printf("Array elements not integers, unknown type.\n");
					}
				} 

				sprintf(global_address_str, "0x%x",arg->symnode->s.v.offset_of_frame_pointer);
				to_return = strdup(global_address_str);	
			} else {

				/* get offset within globals */
				int offset = arg->symnode->s.v.offset_of_frame_pointer;

				if (arg->int_literal >= 0) {
					if (arg->symnode->s.v.type == INT_TS) {
						offset += arg->int_literal * TYPE_SIZE(INT_TS);
					} else {
						printf("Array elements not integers, unknown type.\n");
					}
				} 

				int len = floor(log10(abs(INT_MAX))) + 2;
				char int_str[len];
				sprintf(int_str, "%d(%%ebp)", offset);
				to_return = strdup(int_str);
			}
			break;

		case LABEL_Q_ARG:
			printf("label arg: %s\n",arg->label);
			to_return = arg->label;
			break;

		case RETURN_Q_ARG:
			printf("return arg\n");
			to_return = strdup("%eax");
			break;

		case NULL_ARG:
			printf("null arg\n");
			break;

		default:
			break;
	}

	printf("exiting handle quad arg (returning %s)\n",to_return);
	return to_return;
}

/*
 * add a "comment" to ys
 */
void print_nop_comment(FILE * ys_ptr, char * msg, int id) {
	if (!msg || !ys_ptr)
		return;

	fprintf(ys_ptr, "\tnop # (quad %d) -- %s\n",id,msg);
	return;
}

/*
 * before generating code, set all your frame pointer offsets for variables and put globals in place
 *
 * call this on symtab with `set_variable_memory_locations(symtab);`
 *
 * returns the address where the stack pointer should be set before execution
 */
int set_variable_memory_locations(symboltable_t * symtab) {
	if (!symtab) {
		fprintf(stderr,"cannot set memory locations when symboltable is null!\n");
		return -1;
	}

	symhashtable_t * global_scope = symtab->root;

	/* set all global variable symbols */
	// int globals_size = 0;
	int bottom_of_globals = STK_TOP - TYPE_SIZE(INT_TS) + 1; 	// add one for correct indexing 
	symnode_t * sym;
	for (int i = 0; i < global_scope->size; i++) {
		if (global_scope->table[i] != NULL) {
			sym = global_scope->table[i];
			while (sym != NULL) {

				/* for all global variables */
				if (sym->sym_type == VAR_SYM) {
					if (sym->s.v.modifier == SINGLE_DT) {

						/* put single variable top of global list */
						sym->s.v.offset_of_frame_pointer = bottom_of_globals; 	// not an offset for globals - actual address
						sym->s.v.specie = GLOBAL_VAR;						
						bottom_of_globals -= TYPE_SIZE(sym->s.v.type);						
					} else {

						/* put array in global space */
						int bytes = sym->origin->left_child->right_sibling->value_int * TYPE_SIZE(sym->s.v.type);
						bottom_of_globals -= bytes;
						sym->s.v.offset_of_frame_pointer = bottom_of_globals;
						sym->s.v.specie = GLOBAL_VAR;
					}

				}
				sym = sym->next; 	// get next global variable				
			}
		}
	}

	/* for each function scope, set parameters, locals and temps locations in reference to the FP */
	int function_stk_offset;
	for (symhashtable_t * child = symtab->root->child; child != NULL; child = child->rightsib) {
		function_stk_offset = set_fp_offsets(child, 0, TYPE_SIZE(INT_TS));
		child->function_owner->s.f.stk_offset = function_stk_offset;
	}

	int stack_start = bottom_of_globals;
	return stack_start;
}

/*
 * called ONCE on the function scope table and then it explores down and sets variables
 *
 * returns lowest offset used
 */
int set_fp_offsets(symhashtable_t * symhash, int local_bytes, int param_bytes) {
	if (!symhash)
		return local_bytes;

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

	int lowest_offset_seen = local_bytes;

	int new_offset;
	for (symhashtable_t * sub_scope = symhash->child; sub_scope != NULL; sub_scope = sub_scope->rightsib) {
		new_offset = set_fp_offsets(sub_scope, local_bytes, param_bytes);
		if (new_offset < lowest_offset_seen)
			lowest_offset_seen = new_offset;
	}
	
	return lowest_offset_seen;
}


