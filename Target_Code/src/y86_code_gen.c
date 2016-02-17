#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include "symtab.h"

#include "y86_code_gen.h"

void print_code(quad * to_translate, FILE * ys_file_ptr) {
	switch (to_translate->op) {
		case ADD_Q:
			break;

		case SUB_Q:
			break;

		case MUL_Q:
			break;

		case DIV_Q:
			break;

		case MOD_Q:
			break;

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
			break;

		case EPILOG_Q:
			break;

		case PRECALL_Q:
			break;

		case POSTRET_Q:
			break;

		case PARAM_Q:
			break;

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
		case NULL_ARG:
			break;

		case INT_LITERAL_Q_ARG:
			{
				char int_str[INT_MAX + 2];
				sprintf(int_str, "%d", arg->int_literal);
				to_return = strdup(int_str);
				break;
			}

		case TEMP_VAR_Q_ARG:
			{
				// int fp_offset = arg->temp->temp_symnode->s.v.offset_of_frame_pointer;
				int fp_offset = ((symnode_t *) arg->temp->temp_symnode)->s.v.offset_of_frame_pointer;

				char int_str[INT_MAX + 2];
				sprintf(int_str, "%d", fp_offset);
				to_return = strdup(int_str);
				break;
			}

		case SYMBOL_VAR_Q_ARG:
			break;

		case SYMBOL_ARR_Q_ARG:
			break;

		case LABEL_Q_ARG:
			to_return = arg->label;
			break;

		case RETURN_Q_ARG:
			break;

		default:
			break;
	}

	return to_return;
}