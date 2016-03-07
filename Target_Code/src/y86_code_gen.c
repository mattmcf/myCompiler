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

	/* 
	 * make sure a main is called 
	 */
	symnode_t * main = find_in_top_symboltable(symtab, "main");
	if (!main) {
		fprintf(stderr,"Error during .ys construction. No \"main\" function is declared -- cannot find entry point.\n");
		exit(1);
	} else if (main->sym_type != FUNC_SYM) {
		fprintf(stderr,"Error during .ys construction. No \"main\" function is declared -- cannot find entry point.\n");
		exit(1);		
	}

	/* 
	 * stack and base pointer initialization 
	 */
	int stk_start = set_variable_memory_locations(symtab);
	printf("stack starks at %x\n",stk_start);
	fprintf(ys_fp,".pos 0\n");	
	print_nop_comment(ys_fp, "initialization", -1);
	fprintf(ys_fp,"\tirmovl 0x%x, %%esp\n",stk_start);
	fprintf(ys_fp,"\trrmovl %%esp, %%ebp\n");
	fprintf(ys_fp,"\tirmovl $4, %%eax\n");
	fprintf(ys_fp,"\tsubl %%eax, %%esp\n");

	/* 
	 * initialize globals here 
	 */
	int i;
	for (i = 0; quad_list->arr[i]->op == ASSIGN_Q; i++) {
		printf("global initialization quad %d\n",i);
		print_code(quad_list->arr[i], ys_fp);
	}

	/* 
	 * jump into main when executing 
	 */
	fprintf(ys_fp, "\tcall main\n");
	fprintf(ys_fp, "\thalt\n");

	/* 
	 * translate quad list 
	 */
	for (/* start at end of global initalizations */; i < quad_list->count; i++) {
		printf("looking at quad %d\n",i);
		print_code(quad_list->arr[i], ys_fp);
	}

	/* 
	 * add string constants 
	 */
	for(int i = 0; i < quad_list->count; i++) {
		if (quad_list->arr[i]->op == STRING_Q)
			translate_string(ys_fp, quad_list->arr[i]);
	}

	/* 
	 * wrap up 
	 */
	fprintf(ys_fp, "\n\n");
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

			get_source_value(ys_file_ptr, to_translate->args[1], EAX_R);
			get_source_value(ys_file_ptr, to_translate->args[2], EBX_R);
			fprintf(ys_file_ptr, "\taddl %%ebx, %%eax\n");			
			get_dest_value(ys_file_ptr,EAX_R,to_translate->args[0]);
			break;

		case SUB_Q:
			get_source_value(ys_file_ptr, to_translate->args[1], EAX_R);
			get_source_value(ys_file_ptr, to_translate->args[2], EBX_R);
			fprintf(ys_file_ptr, "\tsubl %%ebx, %%eax\n");
			get_dest_value(ys_file_ptr,EAX_R,to_translate->args[0]);
			break;

		case MUL_Q:
			get_source_value(ys_file_ptr, to_translate->args[1], EAX_R);
			get_source_value(ys_file_ptr, to_translate->args[2], EBX_R);
			fprintf(ys_file_ptr, "\tmull %%ebx, %%eax\n");
			get_dest_value(ys_file_ptr,EAX_R,to_translate->args[0]);		
			break;

		case DIV_Q:
			get_source_value(ys_file_ptr, to_translate->args[1], EAX_R);
			get_source_value(ys_file_ptr, to_translate->args[2], EBX_R);
			fprintf(ys_file_ptr, "\tdivl %%ebx, %%eax\n");
			get_dest_value(ys_file_ptr,EAX_R,to_translate->args[0]);
			break;

		case MOD_Q:
			get_source_value(ys_file_ptr, to_translate->args[1], EAX_R);
			get_source_value(ys_file_ptr, to_translate->args[2], EBX_R);
			fprintf(ys_file_ptr, "\tmodl %%ebx, %%eax\n");
			get_dest_value(ys_file_ptr,EAX_R,to_translate->args[0]);
			break;

		case PRE_INC_Q:
			get_source_value(ys_file_ptr, to_translate->args[1], EAX_R);
			get_source_value(ys_file_ptr, to_translate->args[2], EBX_R);
			fprintf(ys_file_ptr, "\taddl %%ebx, %%eax\n");
			// Update variable
			// Return updated return value

			get_dest_value(ys_file_ptr,EAX_R,to_translate->args[1]);
			get_dest_value(ys_file_ptr,EAX_R,to_translate->args[0]);
			break;

		case PRE_DEC_Q:
			get_source_value(ys_file_ptr, to_translate->args[1], EAX_R);
			get_source_value(ys_file_ptr, to_translate->args[2], EBX_R);
			fprintf(ys_file_ptr, "\tsubl %%ebx, %%eax\n");
			// Update variable
			// Return updated return value

			get_dest_value(ys_file_ptr,EAX_R,to_translate->args[1]);
			get_dest_value(ys_file_ptr,EAX_R,to_translate->args[0]);			
			break;

		case POST_INC_Q:
			get_source_value(ys_file_ptr,to_translate->args[1],EAX_R);
			// Return variable's original value
			get_dest_value(ys_file_ptr,EAX_R,to_translate->args[0]);

			get_source_value(ys_file_ptr,to_translate->args[2],EBX_R);
			fprintf(ys_file_ptr, "\taddl %%ebx, %%eax\n");
			// Update variable
			get_dest_value(ys_file_ptr, EAX_R,to_translate->args[1]);
			break;

		case POST_DEC_Q:
			get_source_value(ys_file_ptr,to_translate->args[1],EAX_R);
			// Return variable's original value
			get_dest_value(ys_file_ptr,EAX_R,to_translate->args[0]);

			get_source_value(ys_file_ptr,to_translate->args[2],EBX_R);
			fprintf(ys_file_ptr, "\tsubl %%ebx, %%eax\n");
			// Update variable
			get_dest_value(ys_file_ptr, EAX_R,to_translate->args[1]);
			break;

		case NOT_Q:
			get_source_value(ys_file_ptr,to_translate->args[0],EAX_R);
			fprintf(ys_file_ptr, "\tirmovl $0, %%ebx\n");
			fprintf(ys_file_ptr, "\tsubl %%ebx, %%eax\n");

			fprintf(ys_file_ptr, "\tirmovl $1, %%ebx\n");
			fprintf(ys_file_ptr, "\tcmove %%ebx, %%eax\n");
			fprintf(ys_file_ptr, "\tirmovl $0, %%ebx\n");
			fprintf(ys_file_ptr, "\tcmovne %%ebx, %%eax\n");
			get_dest_value(ys_file_ptr,EAX_R,to_translate->args[0]);
			break;

		case NEG_Q:
			// Negative of an integer n = 0 - n
			// i.e. 0 - 1 = -1, 0 - (-1) = 1
			fprintf(ys_file_ptr, "\tmrmovl $0, %%eax\n");
			get_source_value(ys_file_ptr,to_translate->args[0],EBX_R);
			fprintf(ys_file_ptr, "\tsubl %%ebx, %%eax\n");
			get_dest_value(ys_file_ptr,EAX_R,to_translate->args[0]);
			break;

		case ASSIGN_Q:
			print_nop_comment(ys_file_ptr, "assignment", to_translate->number);

			get_source_value(ys_file_ptr,to_translate->args[1],EAX_R);
			get_dest_value(ys_file_ptr,EAX_R,to_translate->args[0]);
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
				get_source_value(ys_file_ptr,to_translate->args[0],EBX_R);
				fprintf(ys_file_ptr, "\tsubl %%ebx, %%eax\n");
				fprintf(ys_file_ptr, "\tje %s\n", label);

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
					get_source_value(ys_file_ptr,to_translate->args[0], EAX_R);
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
			get_dest_value(ys_file_ptr,EAX_R,to_translate->args[0]);
			break;

		case SIZEOF_Q:
			{
				int var_size = to_translate->args[1]->symnode->s.v.byte_size;
				fprintf(ys_file_ptr, "\tirmovl $%d, %%eax\n", var_size);
				get_dest_value(ys_file_ptr,EAX_R,to_translate->args[0]);
				break;
			}

		case PROLOG_Q:
			{
				print_nop_comment(ys_file_ptr, "function prolog", to_translate->number);
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

			fprintf(ys_file_ptr, "\trrmovl %%ebp, %%esp\n");
			fprintf(ys_file_ptr, "\tpopl %%ebp\n"); 						// return to old frame pointer
			fprintf(ys_file_ptr, "\tret\n");
			break;

		case PRECALL_Q:
			print_nop_comment(ys_file_ptr, "function precall", to_translate->number);
			fprintf(ys_file_ptr, "\tcall %s\n",to_translate->args[0]->label); 								// pushes ret addr on stack				
			break;

		case POSTRET_Q:
			// return value is in %eax	
			{
				print_nop_comment(ys_file_ptr, "post return", to_translate->number);

				/* make a post return label */
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
				get_source_value(ys_file_ptr,to_translate->args[0],EAX_R);
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
				get_source_value(ys_file_ptr,to_translate->args[0],EAX_R);

			// variable return
			} else {
				get_source_value(ys_file_ptr,to_translate->args[0],EAX_R);
			}
			break;

		case STRING_Q:
			/* update -- wait to do this until all the text has been translated. */
			// {
			// 	int len = strlen(to_translate->args[0]->label);
			// 	//fprintf(ys_file_ptr,"\t.align 4\n");
			// 	for (int i = 0; i < len; i++) {
			// 		fprintf(ys_file_ptr,"\t.byte 0x%x\n",to_translate->args[0]->label[i]);
			// 	}
			// 	fprintf(ys_file_ptr,"\t.byte 0x0\n"); 	// null terminator
			// }
			break;

		case LABEL_Q:			
			fprintf(ys_file_ptr,"%s:\n",to_translate->args[0]->label);
			break;

		default:
			break;
	}
}

int get_source_value(FILE * fp, quad_arg * src, my_register_t dest) {
	if (!src || !fp)
		return 1;

	printf("getting source\n");
	switch(src->type) {
		case INT_LITERAL_Q_ARG:
			printf("constant %d\n",src->int_literal);		
			fprintf(fp, "\tirmovl 0x%x, %s\n",src->int_literal,REGISTER_STR(dest));
			break;

		case TEMP_VAR_Q_ARG:
			printf("temp variable symbol %s\n", ((symnode_t *) src->temp->temp_symnode)->name);
			fprintf(fp, "\tmrmovl $%d(%%ebp), %s\n", ((symnode_t *) src->temp->temp_symnode)->s.v.offset_of_frame_pointer, REGISTER_STR(dest));
			break;

		case SYMBOL_VAR_Q_ARG:
			printf("variable symbol %s\n",src->symnode->name);
			if (src->symnode->s.v.specie == GLOBAL_VAR) {
				/* return absolute address */
				fprintf(fp,"\tmrmovl 0x%x, %s\n",src->symnode->s.v.offset_of_frame_pointer, REGISTER_STR(dest));

			} else {
				/* return relative address */
				fprintf(fp,"\tmrmovl $%d(%%ebp), %s\n", src->symnode->s.v.offset_of_frame_pointer, REGISTER_STR(dest));
			}
			break;			

		case SYMBOL_ARR_Q_ARG: 
			printf("array symbol %s, offset %d\n",src->symnode->name, src->temp->id);
			{
				/*
				 * get array head
				 */
				if (src->symnode->s.v.specie == GLOBAL_VAR)	{					// get absolute address of pointer if global
					fprintf(fp,"\tirmovl 0x%x, %%edi\n",src->symnode->s.v.offset_of_frame_pointer);

				} else if (src->symnode->s.v.offset_of_frame_pointer > 0) {		// need get address of array from parameters
					fprintf(fp,"\tmrmovl $%d(%%ebp), %%edi\n", src->symnode->s.v.offset_of_frame_pointer);

				} else {														// else get relative address based addition to FP 
					fprintf(fp,"\trrmovl %%ebp, %%edi\n");
					fprintf(fp,"\tirmovl $%d, %%ebx\n",src->symnode->s.v.offset_of_frame_pointer);
					fprintf(fp,"\taddl %%ebx, %%edi\n");
				}

				/*
				 * calculate offset
				 */
				if (src->int_literal != PASS_ARR_POINTER) {
					/* get temp that holds index */
					fprintf(fp,"\tmrmovl $%d(%%ebp), %%ebx\n",((symnode_t *) src->temp->temp_symnode)->s.v.offset_of_frame_pointer);
					fprintf(fp,"\tshll $2, %%ebx\n");
					fprintf(fp,"\taddl %%ebx, %%edi\n");
					fprintf(fp,"\tmrmovl (%%edi), %s\n", REGISTER_STR(dest));					
				} else {
					/* pass array pointer */
					fprintf(fp,"\trrmovl %%edi, %s\n", REGISTER_STR(dest));
				}					
			}
			break;

		case LABEL_Q_ARG:
			printf("label %s\n",src->label);
			fprintf(fp,"%s",src->label);
			break;

		case RETURN_Q_ARG:
			printf("return arg\n");
			fprintf(fp,"\trrmovl %%eax, %s\n",REGISTER_STR(dest));
			break;

		default:
			fprintf(stderr,"seeing null arguments / getting value from bad arg\n");
			exit(1);
			break;
	}
	return 0;
}


int get_dest_value(FILE * fp, my_register_t src, quad_arg * dest) {
	if (!dest || !fp)
		return 1;

	printf("getting destination value\n");
	switch(dest->type){
		case TEMP_VAR_Q_ARG:
			printf("temp variable symbol %s\n", ((symnode_t *) dest->temp->temp_symnode)->name);
			fprintf(fp,"\trmmovl %s, $%d(%%ebp)\n",REGISTER_STR(src),((symnode_t *) dest->temp->temp_symnode)->s.v.offset_of_frame_pointer);
			break;

		case SYMBOL_VAR_Q_ARG:
			printf("variable symbol\n");
			if (dest->symnode->s.v.specie == GLOBAL_VAR) {
				/* return absolute address */
				fprintf(fp,"\trmmovl %s, 0x%x\n",REGISTER_STR(src), dest->symnode->s.v.offset_of_frame_pointer);

			} else {
				/* return relative address */
				fprintf(fp,"\trmmovl %s, $%d(%%ebp)\n", REGISTER_STR(src), dest->symnode->s.v.offset_of_frame_pointer);
			}
			break;			

		case SYMBOL_ARR_Q_ARG:

			/* 
			 * get array pointer into %edi 
			 */
			if (dest->symnode->s.v.specie == GLOBAL_VAR)	{					// get absolute address of pointer if global
				fprintf(fp,"\tirmovl 0x%x, %%edi\n",dest->symnode->s.v.offset_of_frame_pointer);
				//sprintf(t1,"0x%x, %%edi\n",src->symnode->s.v.offset_of_frame_pointer);

			} else if (dest->symnode->s.v.offset_of_frame_pointer > 0) {		// need get address out of memory for parameter
				fprintf(fp,"\tmrmovl $%d(%%ebp), %%edi\n", dest->symnode->s.v.offset_of_frame_pointer);

			} else {															// else get relative address based addition to FP 
				fprintf(fp,"\trrmovl %%ebp, %%edi\n");
				fprintf(fp,"\tirmovl $%d, %%ebx\n",dest->symnode->s.v.offset_of_frame_pointer);
				fprintf(fp,"\taddl %%ebx, %%edi\n");
			}

			/* 
			 * calculate offset
			 */
			if (dest->int_literal != PASS_ARR_POINTER && dest->temp != NULL) {
				/* get temp that holds index */
				fprintf(fp,"\tmrmovl $%d(%%ebp), %%ebx\n",((symnode_t *) dest->temp->temp_symnode)->s.v.offset_of_frame_pointer);
				fprintf(fp,"\tshll $2, %%ebx\n");
				fprintf(fp,"\taddl %%ebx, %%edi\n");
				fprintf(fp,"\trmmovl %s, (%%edi)\n", REGISTER_STR(src));					
			} else {
				/* 
				 * For how we work with arrays, can never actually change array head
				 */
				fprintf(stderr,"error during code generation: cannot assign new values to array headers\n");
				exit(1);
			}
			break;

		case RETURN_Q_ARG:
			printf("return destination\n");
			fprintf(fp,"\trrmovl %s, %%eax\n",REGISTER_STR(src));
			break;

		case LABEL_Q_ARG:			
		case INT_LITERAL_Q_ARG:	
		default:
			fprintf(stderr,"destination argument doesn't make sense\n");
			exit(1);
			break;
	}

	return 0;
}

void comp_sub(quad * to_translate, FILE * ys_file_ptr) {
	get_source_value(ys_file_ptr,to_translate->args[1],EAX_R);
	get_source_value(ys_file_ptr,to_translate->args[2],EBX_R);
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

	get_dest_value(ys_file_ptr,EAX_R, to_translate->args[0]);
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

void translate_string(FILE * ys_file_ptr, quad * string_to_add) {
	if (!ys_file_ptr || ! string_to_add)
		return;

	if (!string_to_add->args[0] || !string_to_add->args[1]) {
		fprintf(stderr,"cannot translate string -- lacking arguments\n");
		exit(1);
	}

	// generate label
	fprintf(ys_file_ptr,"%s:\n",string_to_add->args[0]->label);

	// generate ascii bytes

	int len = strlen(string_to_add->args[1]->label);
	for (int i = 0; i < len; i++) {
		fprintf(ys_file_ptr,"\t.byte 0x%x\n",string_to_add->args[1]->label[i]);
	}
	fprintf(ys_file_ptr,"\t.byte 0x0\n"); 	// null terminator

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


