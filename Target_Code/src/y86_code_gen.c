#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "symtab.h"
#include "y86_code_gen.h"

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