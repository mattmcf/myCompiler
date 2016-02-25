#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "symtab.h"
#include "IR_gen.h"
#include "y86_code_gen.h"
#include "types.h"

#define MAX_ARG_LEN 	50
#define MAX_HEX_ADDRESS_LEN 11 	// 0x????????'\n' is 11 characters for a string (with null terminator)
#define STK_TOP 0x0000FFFF


extern symboltable_t * symtab; 	// for global lookups of symbols
extern quad_arr * quad_list; 		// global quad list

#define DSTR_reg 0x00FFFE10 		// DISPLAY STRING DATA REGISTER
#define DHXR_reg 0x00FFFE14			// DISPLAY HEX REGISTER
#define KSTR_reg 0x00FFFE18 		// KEYBOARD STRING REGISTER (BLOCKING) -- SEE DOCUMENTATION
#define KHXR_reg 0x00FFFE1C 		// KEYBOARD HEX REGISTER (BLOCKING)
#define KBDR_reg 0x00FFFE04 		// KEYBAORD DATA REGISTER 

condition_type condition;

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

	/* make sure a main is called */

	symnode_t * main = find_in_top_symboltable(symtab, "main");
	if (!main) {
		fprintf(stderr,"Error during .ys construction. No \"main\" function is declared -- cannot find entry point.\n");
		exit(1);
	} else if (main->sym_type != FUNC_SYM) {
		fprintf(stderr,"Error during .ys construction. No \"main\" function is declared -- cannot find entry point.\n");
		exit(1);		
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

	/* initialize globals here */
	int i;
	for (i = 0; quad_list->arr[i]->op == ASSIGN_Q; i++) {
		printf("global initialziation quad %d\n",i);
		print_code(quad_list->arr[i], ys_fp);
	}

	fprintf(ys_fp, "\tcall main\n");
	fprintf(ys_fp, "\thalt\n");

	/* translate quad list */

	for (/* start at end of global initalizations */; i < quad_list->count; i++) {
		printf("looking at quad %d\n",i);
		print_code(quad_list->arr[i], ys_fp);
	}

	/* wrap up */
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
			print_nop_comment(ys_file_ptr,"add",to_translate->number);

			fprintf(ys_file_ptr,"\t%s, %%eax\n", get_source_value(to_translate->args[1]));
			fprintf(ys_file_ptr,"\t%s, %%ebx\n", get_source_value(to_translate->args[2]));
			fprintf(ys_file_ptr, "\taddl %%ebx, %%eax\n");			
			fprintf(ys_file_ptr,"\trmmovl %%eax, %s\n",get_dest_value(to_translate->args[0]));
			break;

		case SUB_Q:
			fprintf(ys_file_ptr, "\t%s, %%eax\n", get_source_value(to_translate->args[1]));
			fprintf(ys_file_ptr, "\t%s, %%ebx\n", get_source_value(to_translate->args[2]));
			fprintf(ys_file_ptr, "\tsubl %%ebx, %%eax\n");
			fprintf(ys_file_ptr, "\trmmovl %%eax, %s\n", get_dest_value(to_translate->args[0]));
			break;

		case MUL_Q:
			fprintf(ys_file_ptr, "\t%s, %%eax\n", get_source_value(to_translate->args[1]));
			fprintf(ys_file_ptr, "\t%s, %%ebx\n", get_source_value(to_translate->args[2]));
			fprintf(ys_file_ptr, "\tmull %%ebx, %%eax\n");
			fprintf(ys_file_ptr, "\trmmovl %%eax, %s\n", get_dest_value(to_translate->args[0]));
			break;

		case DIV_Q:
			fprintf(ys_file_ptr, "\t%s, %%eax\n", get_source_value(to_translate->args[1]));
			fprintf(ys_file_ptr, "\t%s, %%ebx\n", get_source_value(to_translate->args[2]));
			fprintf(ys_file_ptr, "\tdivl %%ebx, %%eax\n");
			fprintf(ys_file_ptr, "\trmmovl %%eax, %s\n", get_dest_value(to_translate->args[0]));
			break;

		case MOD_Q:
			fprintf(ys_file_ptr, "\t%s, %%eax\n", get_source_value(to_translate->args[1]));
			fprintf(ys_file_ptr, "\t%s, %%ebx\n", get_source_value(to_translate->args[2]));
			fprintf(ys_file_ptr, "\tmodl %%ebx, %%eax\n");
			fprintf(ys_file_ptr, "\trmmovl %%eax, %s\n", get_dest_value(to_translate->args[0]));
			break;

		case INC_Q:
			fprintf(ys_file_ptr, "\t%s, %%eax\n", get_source_value(to_translate->args[0]));
			fprintf(ys_file_ptr, "\t%s, %%ebx\n", get_source_value(to_translate->args[1]));
			fprintf(ys_file_ptr, "\taddl %%ebx, %%eax\n");
			fprintf(ys_file_ptr, "\trmmovl %%eax, %s\n", get_dest_value(to_translate->args[0]));
			break;

		case DEC_Q:
			fprintf(ys_file_ptr, "\t%s, %%eax\n", get_source_value(to_translate->args[0]));
			fprintf(ys_file_ptr, "\t%s, %%ebx\n", get_source_value(to_translate->args[1]));
			fprintf(ys_file_ptr, "\tsubl %%ebx, %%eax\n");
			fprintf(ys_file_ptr, "\trmmovl %%eax, %s\n", get_dest_value(to_translate->args[0]));
			break;

		case NOT_Q:
			fprintf(ys_file_ptr, "\t%s, %%eax\n", get_source_value(to_translate->args[0]));
			fprintf(ys_file_ptr, "\tirmovl $0, %%ebx\n");
			fprintf(ys_file_ptr, "\tsubl %%ebx, %%eax\n");

			fprintf(ys_file_ptr, "\tirmovl $1, %%ebx\n");
			fprintf(ys_file_ptr, "\tcmove %%ebx, %%eax\n");
			fprintf(ys_file_ptr, "\tirmovl $0, %%ebx\n");
			fprintf(ys_file_ptr, "\tcmovne %%ebx, %%eax\n");
			fprintf(ys_file_ptr, "\trmmovl %%eax, %s\n", get_dest_value(to_translate->args[0]));
			break;

		case NEG_Q:
			// Negative of an integer n = 0 - n
			// i.e. 0 - 1 = -1, 0 - (-1) = 1
			fprintf(ys_file_ptr, "\tmrmovl $0, %%eax\n");
			fprintf(ys_file_ptr, "\t%s, %%ebx\n", get_source_value(to_translate->args[0]));
			fprintf(ys_file_ptr, "\tsubl %%ebx, %%eax\n");
			fprintf(ys_file_ptr, "\trmmovl %%eax, %s\n", get_dest_value(to_translate->args[0]));
			break;

		case ASSIGN_Q:
			print_nop_comment(ys_file_ptr, "assignment", to_translate->number);

			fprintf(ys_file_ptr,"\t%s, %%eax\n",get_source_value(to_translate->args[1]));

			if (to_translate->args[0]->type == SYMBOL_ARR_Q_ARG)
				fprintf(ys_file_ptr,"\t%s\n",load_arr_ptr(to_translate->args[0]));

			fprintf(ys_file_ptr,"\trmmovl %%eax, %s\n", get_dest_value(to_translate->args[0]));
			break;

		case LT_Q:
			// SF = 1 and ZF = 0
			// jl
			{
				condition = LT_C;
				comp_sub(to_translate, ys_file_ptr);
				break;
			}

		case GT_Q:
			// SF = 0 and ZF = 0
			// jg
			{
				condition = GT_C;
				comp_sub(to_translate, ys_file_ptr);
				break;
			}

		case LTE_Q:
			// SF = 1 or ZF = 0
			// jle
			{
				condition = LTE_C;
				comp_sub(to_translate, ys_file_ptr);
				break;
			}

		case GTE_Q:
			// SF = 0 or ZF = 1
			// jge
			{
				condition = GTE_C;
				comp_sub(to_translate, ys_file_ptr);
				break;
			}

		case NE_Q:
			// ZF = 0
			// jne
			{
				condition = NE_C;
				comp_sub(to_translate, ys_file_ptr);
				break;
			}

		case EQ_Q:
			// SF = 0 and ZF = 1
			// je
			{
				condition = EQ_C;
				comp_sub(to_translate, ys_file_ptr);
				break;
			}

		case IFFALSE_Q:
			{
				char * label = to_translate->args[1]->label;

				// Just check if temp is 0
				fprintf(ys_file_ptr, "\tirmovl $0, %%eax\n");
				fprintf(ys_file_ptr, "\t%s, %%ebx\n", get_source_value(to_translate->args[0]));
				fprintf(ys_file_ptr, "\tsubl %%ebx, %%eax\n");
				fprintf(ys_file_ptr, "\tje %s\n", label);

				// Use the opposite conditional jump since we are checking
				// for falseness
				// switch (condition) {
				// 	case LT_C:
				// 		fprintf(ys_file_ptr, "\tjge %s\n", label);
				// 		break;
				// 	case GT_C:
				// 		fprintf(ys_file_ptr, "\tjle %s\n", label);
				// 		break;
				// 	case LTE_C:
				// 		fprintf(ys_file_ptr, "\tjg %s\n", label);
				// 		break;
				// 	case GTE_C:
				// 		fprintf(ys_file_ptr, "\tjl %s\n", label);
				// 		break;
				// 	case NE_C:
				// 		fprintf(ys_file_ptr, "\tje %s\n", label);
				// 		break;
				// 	case EQ_C:
				// 		fprintf(ys_file_ptr, "\tjne %s\n", label);
				// 		break;
				// 	case NULL_C:
				// 		// Just check if temp is 0
				// 		fprintf(ys_file_ptr, "\tirmovl $0, %%eax\n");
				// 		fprintf(ys_file_ptr, "\t%s, %%ebx\n", get_source_value(to_translate->args[0]));
				// 		fprintf(ys_file_ptr, "\tsubl %%ebx, %%eax\n");
				// 		fprintf(ys_file_ptr, "\tje %s\n", label);
				// 		break;
				// 	default:
				// 		break;
				// }

				condition = NULL_C;

				break;
			}

		case GOTO_Q:
			print_nop_comment(ys_file_ptr,"goto",to_translate->number);

			//char * jmp_label = (to_translate->args[0]);
			fprintf(ys_file_ptr,"\tjmp %s\n",to_translate->args[0]->label); 	
			break;

		case PRINT_Q:
			print_nop_comment(ys_file_ptr, "printing", to_translate->number);		
			switch(to_translate->args[0]->type){

				/* printing a string */
				case LABEL_Q_ARG: 	
					fprintf(ys_file_ptr, "\tirmovl %s, %%eax\n",to_translate->args[0]->label);
					fprintf(ys_file_ptr, "\trmmovl %%eax, 0x%x\n",DSTR_reg);
					break;

				/* printing a value */
				case SYMBOL_ARR_Q_ARG:
				case SYMBOL_VAR_Q_ARG:
				case TEMP_VAR_Q_ARG: 
					fprintf(ys_file_ptr,"\t%s, %%eax\n", get_source_value(to_translate->args[0]));
					fprintf(ys_file_ptr,"\trmmovl %%eax, 0x%x\n",DHXR_reg);	
					break;

				/* printing a return value */
				case RETURN_Q_ARG:
					fprintf(ys_file_ptr,"\trmmovl %%eax, 0x%x\n",DHXR_reg);
					break;

				default:
					break;
			}
			
			break;

		case READ_Q:
			print_nop_comment(ys_file_ptr, "reading", to_translate->number);
			// Right now only reading integers
			fprintf(ys_file_ptr, "\tmrmovl 0x%x, %%eax\n", KHXR_reg);
			fprintf(ys_file_ptr, "\trmmovl %%eax, %s\n", get_dest_value(to_translate->args[0]));
			break;

		case PROLOG_Q:
			{
				print_nop_comment(ys_file_ptr, "function prolog", to_translate->number);

				//char * t1 = handle_quad_arg(to_translate->args[0]);
				//printf("looking for %s in symboltable\n",t1);
				symnode_t * func_sym = find_in_top_symboltable(symtab, to_translate->args[0]->label);

				// error check -- probably not necessary anymore
				if (!func_sym) {
					fprintf(stderr,"error during code generation: couldn't find function symbol %s\n", to_translate->args[0]->label);
					exit(1);
				}

				fprintf(ys_file_ptr, "%s:\n",to_translate->args[0]->label);
				fprintf(ys_file_ptr, "\tpushl %%ebp\n");			
				fprintf(ys_file_ptr, "\trrmovl %%esp, %%ebp\n"); 		// move esp to ebp
				/* 
				 * --- set esp to bottom of local and temp space --- 
				 * esp should be set to symnode->s.f.stk_offset for function's symbol
				 */
				fprintf(ys_file_ptr, "\tirmovl $%d, %%eax\n",func_sym->s.f.stk_offset);
				fprintf(ys_file_ptr, "\taddl %%eax, %%esp\n");				
			}
			break;

		case EPILOG_Q:
			print_nop_comment(ys_file_ptr, "function epilog", to_translate->number);

			//char * epilog_label = handle_quad_arg(to_translate->args[0]);
			//fprintf(ys_file_ptr, "%s:\n",epilog_label);
			fprintf(ys_file_ptr, "\trrmovl %%ebp, %%esp\n");
			fprintf(ys_file_ptr, "\tpopl %%ebp\n"); 						// return to old frame pointer
			fprintf(ys_file_ptr, "\tret\n");
			break;

		case PRECALL_Q:
			print_nop_comment(ys_file_ptr, "function precall", to_translate->number);
			//char * func_label = handle_quad_arg(to_translate->args[0]); 	// get function label	
			fprintf(ys_file_ptr, "\tcall %s\n",to_translate->args[0]->label); 								// pushes ret addr on stack				
			break;

		case POSTRET_Q:
			// return value is in %eax	
			{
				print_nop_comment(ys_file_ptr, "post return", to_translate->number);

				/* make a post return label */
				//char * t1 = handle_quad_arg(to_translate->args[0]); 								
				symnode_t * func_sym = find_in_top_symboltable(symtab, to_translate->args[0]->label);	

				/* use control link to get back to caller frame */	
				fprintf(ys_file_ptr, "\tirmovl $%d, %%ebx\n",func_sym->s.f.stk_offset); 	// %ebx b/c return lives in %eax
				fprintf(ys_file_ptr, "\tsubl %%ebx, %%esp\n");								
			}
			break;

		case PARAM_Q:

			print_nop_comment(ys_file_ptr,"parameter",to_translate->number);	 	
			if (to_translate->args[0]->type == SYMBOL_ARR_Q_ARG && to_translate->args[0]->int_literal == PASS_ARR_POINTER) {
				/* passing array pointer */

				/* if global? */ 
				if (to_translate->args[0]->symnode->s.v.specie == GLOBAL_VAR) {
					/* return absolute address */
					fprintf(ys_file_ptr,"\tirmovl 0x%x, %%eax\n",to_translate->args[0]->symnode->s.v.offset_of_frame_pointer);
				} else {
					/* calculate absolute address by adding offset (in bytes) of ebp */
					fprintf(ys_file_ptr,"\tirmovl $%d, %%eax\n", to_translate->args[0]->symnode->s.v.offset_of_frame_pointer);
					fprintf(ys_file_ptr,"\taddl %%ebp, %%eax\n");
				}

			} else {
				/* passing single parameter */
				//char * t1 = handle_quad_arg(to_translate->args[0]);
				fprintf(ys_file_ptr,"\t%s, %%eax\n",get_source_value(to_translate->args[0]));
				//fprintf(ys_file_ptr, "\t%s %s, %%eax\n", get_move_type(to_translate->args[0]),t1);
			}

			fprintf(ys_file_ptr, "\tpushl %%eax\n");			
			break;

		case RET_Q:
			print_nop_comment(ys_file_ptr, "return statement", to_translate->number);

			// void return
			if (to_translate->args[0] == NULL) {
				fprintf(ys_file_ptr, "\tirmovl $0, %%eax\n"); 	// clear return value for void

			// constant return
			} else if (to_translate->args[0]->type == INT_LITERAL_Q_ARG) {
				fprintf(ys_file_ptr, "\t%s, %%eax\n", get_source_value(to_translate->args[0]));

			// variable return
			} else {
				//fprintf(ys_file_ptr, "\tmrmovl %s, %%eax\n", handle_quad_arg(to_translate->args[0]));
				fprintf(ys_file_ptr,"\t%s, %%eax\n",get_source_value(to_translate->args[0]));
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
			//char * t1 = handle_quad_arg(to_translate->args[0]); 			// get string for label
			fprintf(ys_file_ptr,"%s:\n",to_translate->args[0]->label);
			break;

		default:
			break;
	}
}

char * load_arr_ptr(quad_arg * arr) {
	if (!arr || arr->type != SYMBOL_ARR_Q_ARG)
		return NULL;

	printf("loading array pointer into %%edi\n");
	char load_str[MAX_ARG_LEN * 2] = "";
	if (arr->symnode->s.v.specie == GLOBAL_VAR)
		sprintf(load_str,"irmovl 0x%x, %%edi",arr->symnode->s.v.offset_of_frame_pointer);

	else if (arr->symnode->s.v.offset_of_frame_pointer > 0) 	// parameters are passed as pointers, so just load that
		sprintf(load_str,"mrmovl $%d(%%ebp), %%edi\n",arr->symnode->s.v.offset_of_frame_pointer);

	else
		sprintf(load_str,"irmovl $%d, %%edi\n\taddl %%ebp, %%edi",arr->symnode->s.v.offset_of_frame_pointer);
		//sprintf(load_str,"mrmovl $%d(%%ebp), %%edi",arr->symnode->s.v.offset_of_frame_pointer);

	return strdup(load_str);
}

char * get_source_value(quad_arg * src) {
	if (!src)
		return NULL;

	printf("getting source\n");

	char src_str[MAX_ARG_LEN * 10] = "";
	strcat(src_str,get_move_type(src));
	strcat(src_str," ");

	char t1[MAX_ARG_LEN * 5] = "";
	switch(src->type) {
		case INT_LITERAL_Q_ARG:
			printf("constant %d\n",src->int_literal);
			sprintf(t1,"0x%x",src->int_literal);
			break;

		case TEMP_VAR_Q_ARG:
			printf("temp variable symbol %s\n", ((symnode_t *) src->temp->temp_symnode)->name);
			sprintf(t1,"$%d(%%ebp)",((symnode_t *) src->temp->temp_symnode)->s.v.offset_of_frame_pointer); 	// always local
			break;

		case SYMBOL_VAR_Q_ARG:
			printf("variable symbol %s\n",src->symnode->name);
			if (src->symnode->s.v.specie == GLOBAL_VAR) {
				/* return absolute address */
				sprintf(t1, "0x%x",src->symnode->s.v.offset_of_frame_pointer); 	// global address

			} else {
				/* return relative address */
				sprintf(t1, "%d(%%ebp)", src->symnode->s.v.offset_of_frame_pointer);			
			}
			break;			

		case SYMBOL_ARR_Q_ARG: 
			printf("array symbol %s, offset %d\n",src->symnode->name, src->int_literal);
			{
				if (src->symnode->s.v.specie == GLOBAL_VAR)		// get absolute address of pointer if global
					sprintf(t1,"0x%x, %%edi\n",src->symnode->s.v.offset_of_frame_pointer);	
				else											// else get relative address based on FP 
					sprintf(t1,"%%ebp, %%edi\n\tirmovl $%d, %%ebx\n\taddl %%ebx, %%edi\n",src->symnode->s.v.offset_of_frame_pointer);

				/* pointer now lives in %edi, now we need to get the offset into %eax */
				char t2[MAX_ARG_LEN * 5] = "";
				if (src->int_literal >= 0 && src->index) {
					printf("getting index from symbol / constant\n");

					switch(src->index->type){
						case INT_LITERAL_Q_ARG:
							printf("constant index\n");
							sprintf(t2,"\tmrmovl %d(%%edi)",src->index->int_literal * TYPE_SIZE(src->symnode->s.v.type));
							break;

							/*
							 * step 1) // get symbol's value into %eax
							 * step 1.5) // shift left by 2 to multiply by 4
							 * step 2) // add that value to the array's pointer
							 * step 3) // get indexed value from memory
							 */
						case SYMBOL_VAR_Q_ARG:
							printf("symbol index\n");
							sprintf(t2,"\tmrmovl %d(%%ebp), %%eax\n\tshll $2, %%eax\n\taddl %%eax, %%edi\n\tmrmovl (%%edi)", 
								src->index->symnode->s.v.offset_of_frame_pointer);
							break;

						case TEMP_VAR_Q_ARG:
							printf("temp index");
							sprintf(t2,"\tmrmovl %d(%%ebp), %%eax\n\tshll $2, %%eax\n\taddl %%eax, %%edi\n\tmrmovl (%%edi)", 
								((symnode_t *)src->index->temp->temp_symnode)->s.v.offset_of_frame_pointer);
							break;

						/* not handled yet */
						case SYMBOL_ARR_Q_ARG:
							printf("Arrays indexing into arrays is not supported.\n");
							exit(1);
							break;

						default:
							printf("unknown index into array\n");
							exit(1);
							break;
					}		
					strcat(t1,t2);			
				} else {
					printf("passing pointer\n");

					// pass pointer
					strcat(t1,"%%edi");
				}
					
			}
			break;

		case LABEL_Q_ARG:
			printf("label %s\n",src->label);
			strcat(t1,src->label);
			break;

		case RETURN_Q_ARG:
			printf("return arg\n");
			sprintf(t1,"%%eax");
			break;

		default:
			fprintf(stderr,"seeing null arguments / getting value from bad arg\n");
			exit(1);
			break;
	}

	strcat(src_str,t1);

	printf("got source value %s\n",src_str);
	return strdup(src_str);
}

/*
 * assumes that array pointer is already in %edi
 */
char * get_dest_value(quad_arg * dest) {
	if (!dest)
		return NULL;

	char dest_str[MAX_ARG_LEN * 5] = "";
	printf("getting destination value\n");

	switch(dest->type){
		case TEMP_VAR_Q_ARG:
			printf("temp variable symbol %s\n", ((symnode_t *) dest->temp->temp_symnode)->name);
			sprintf(dest_str,"$%d(%%ebp)",((symnode_t *) dest->temp->temp_symnode)->s.v.offset_of_frame_pointer); 	// always local
			break;

		case SYMBOL_VAR_Q_ARG:
			printf("variable symbol\n");
			//printf("variable symbol %s\n",dest->symnode->name);
			if (dest->symnode->s.v.specie == GLOBAL_VAR) {
				/* return absolute address */
				sprintf(dest_str, "0x%x",dest->symnode->s.v.offset_of_frame_pointer); 	// global address

			} else {
				/* return relative address */
				sprintf(dest_str, "$%d(%%ebp)", dest->symnode->s.v.offset_of_frame_pointer);			
			}
			break;			

		case SYMBOL_ARR_Q_ARG:
			printf("array symbol %s, offset %s\n",dest->symnode->name, get_quad_arg_label(dest->index) ); 
			if (dest->int_literal >= 0 && dest->index != NULL) {					// get offset from array pointer
				printf("getting destination index from symbol / constant\n");

				char get_array[MAX_ARG_LEN * 5];
				switch(dest->index->type){
					case INT_LITERAL_Q_ARG:
						printf("constant index\n");
						sprintf(get_array,"%d(%%edi)",dest->index->int_literal * TYPE_SIZE(dest->symnode->s.v.type));
						break;

					/*
					 * step 0) stash %eax source value at $-4(%esp) which is a garbage spot. Nothing's there.
					 * step 1) move offset into register
					 * step 1.5) shift left by two to multiply by 4! (size)
					 * step 2) add that value to the array's pointer
					 * step 3) // get indexed value from memory
					 */
					case SYMBOL_VAR_Q_ARG:
						printf("symbol index\n");
						sprintf(get_array,"$-4(%%esp)\n\tmrmovl %d(%%ebp), %%ebx\n\tshll 2, %%ebx\n\taddl %%ebx, %%edi\n\trmmovl %%eax, (%%edi)", 
							dest->index->symnode->s.v.offset_of_frame_pointer);
						break;

					case TEMP_VAR_Q_ARG:
						printf("temp index");
						sprintf(get_array,"$-4(%%esp)\n\tmrmovl %d(%%ebp), %%ebx\n\tshll 2, %%ebx\n\taddl %%ebx, %%edi\n\trmmovl %%eax, (%%edi)", 
							((symnode_t *)dest->index->temp->temp_symnode)->s.v.offset_of_frame_pointer);
						break;

					/* not handled yet */
					case SYMBOL_ARR_Q_ARG:
						printf("Arrays indexing into arrays is not supported.\n");
						exit(1);
						break;

					default:
						printf("unknown index into array\n");
						exit(1);
						break;
				}
				strcat(dest_str, get_array);		
			} else											// else pass array pointer
				strcat(dest_str,"%%edi");
			break;

		case RETURN_Q_ARG:
			printf("return destination\n");
			sprintf(dest_str,"%%eax");
			break;

		case LABEL_Q_ARG:			
		case INT_LITERAL_Q_ARG:	
		default:
			fprintf(stderr,"destination argument doesn't make sense\n");
			exit(1);
			break;
	}

	printf("returning destination %s\n",dest_str);
	return strdup(dest_str);
}

char * get_move_type(quad_arg * src) {
	if (!src)
		return NULL;

	char * mov_str;

	// immediately load into register
	if (src->type == INT_LITERAL_Q_ARG ||	 									// is constant or
	 (src->type == SYMBOL_ARR_Q_ARG && (src->symnode != NULL) && (src->symnode->s.v.specie == GLOBAL_VAR)) ) { 	// is global
		mov_str = strdup("irmovl");

	// move from register
	} else if (src->type == RETURN_Q_ARG || (src->type == SYMBOL_ARR_Q_ARG) ) {
		mov_str = strdup("rrmovl");

	// load from memory
	} else {
		mov_str = strdup("mrmovl");
	}

	return mov_str;
}

void comp_sub(quad * to_translate, FILE * ys_file_ptr) {
	fprintf(ys_file_ptr, "\t%s, %%eax\n", get_source_value(to_translate->args[1]));
	fprintf(ys_file_ptr, "\t%s, %%ebx\n", get_source_value(to_translate->args[2]));
	fprintf(ys_file_ptr, "\tsubl %%ebx, %%eax\n");

	switch (condition) {
		case LT_C:
			fprintf(ys_file_ptr, "\tirmovl $1, %%ebx\n");
			fprintf(ys_file_ptr, "\tcmovl %%ebx, %%eax\n");
			fprintf(ys_file_ptr, "\tirmovl $0, %%ebx\n");
			fprintf(ys_file_ptr, "\tcmovge %%ebx, %%eax\n");
			break;
		case GT_C:
			fprintf(ys_file_ptr, "\tirmovl $1, %%ebx\n");
			fprintf(ys_file_ptr, "\tcmovg %%ebx, %%eax\n");
			fprintf(ys_file_ptr, "\tirmovl $0, %%ebx\n");
			fprintf(ys_file_ptr, "\tcmovle %%ebx, %%eax\n");
			break;
		case LTE_C:
			fprintf(ys_file_ptr, "\tirmovl $1, %%ebx\n");
			fprintf(ys_file_ptr, "\tcmovle %%ebx, %%eax\n");
			fprintf(ys_file_ptr, "\tirmovl $0, %%ebx\n");
			fprintf(ys_file_ptr, "\tcmovg %%ebx, %%eax\n");
			break;
		case GTE_C:
			fprintf(ys_file_ptr, "\tirmovl $1, %%ebx\n");
			fprintf(ys_file_ptr, "\tcmovge %%ebx, %%eax\n");
			fprintf(ys_file_ptr, "\tirmovl $0, %%ebx\n");
			fprintf(ys_file_ptr, "\tcmovl %%ebx, %%eax\n");
			break;
		case NE_C:
			fprintf(ys_file_ptr, "\tirmovl $1, %%ebx\n");
			fprintf(ys_file_ptr, "\tcmovne %%ebx, %%eax\n");
			fprintf(ys_file_ptr, "\tirmovl $0, %%ebx\n");
			fprintf(ys_file_ptr, "\tcmove %%ebx, %%eax\n");
			break;
		case EQ_C:
			fprintf(ys_file_ptr, "\tirmovl $1, %%ebx\n");
			fprintf(ys_file_ptr, "\tcmove %%ebx, %%eax\n");
			fprintf(ys_file_ptr, "\tirmovl $0, %%ebx\n");
			fprintf(ys_file_ptr, "\tcmovne %%ebx, %%eax\n");
			break;
		default:
			break;
	}

	fprintf(ys_file_ptr, "\trmmovl %%eax, %s\n", get_dest_value(to_translate->args[0]));
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
	int bottom_of_globals = (STK_TOP + 1) - TYPE_SIZE(INT_TS); 	// add one for correct indexing 
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
						int bytes = (sym->origin->left_child->right_sibling->value_int - 1)* TYPE_SIZE(sym->s.v.type); 	// minus one because bottom already points to a free spot
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


