/*
 * Yondon Fu and Matt McFarland
 * CS57 - 16W
 * 
 * Token definition file. 
 * Inspired by SWS's toktype.h 
 */

#ifndef TOKTYPES_H
#define TOKTYPES_H

enum TokTypes { ID_T = 258, INT_T, STRING_T, TYPEINT_T,
			IF_T, ELSE_T, WHILE_T, RETURN_T, FOR_T,
			VOID_T, READ_T, PRINT_T,
			ADD_T, SUB_T, MULT_T, DIV_T, ASSIGN_T,
			LT_T, GT_T, LTE_T, GTE_T, EQ_T, NE_T,
			INCR_T, DECR_T, 
			AND_T, OR_T, NOT_T,
			SEMIC_T, COMMA_T, OPAREN_T, CPAREN_T, 
			OSQR_T, CSQR_T, OCRLY_T, CCRLY_T,
			PCENT_T, EOF_T, COMMENT_T, OTHER_T };
			
static char * tok_names[] =
	{ "ID", "INT", "STRING", "TYPEINT",
	"IF", "ELSE", "WHILE", "RETURN", "FOR",
	"VOID", "READ", "PRINT",
	"PLUS", "MINUS", "MULTIPLY", "DIVIDE", "ASSIGN",
	"LESS", "GREATER", "LESSEQUAL", "GREATEREQUAL", "ISEQUAL", "NOTEQUAL",
	"INCREMENT", "DECREMENT", 
	"AND", "OR", "NOT",
	"SEMICOLON", "COMMA", "OPAREN", "CPAREN", 
	"OSQR", "CSQR", "OCRLY", "CCRLY",
	"PCENT", "EOF", "COMMENT", "OTHER" };

#define FIRST_BIG_TOKEN ID_T

#endif
