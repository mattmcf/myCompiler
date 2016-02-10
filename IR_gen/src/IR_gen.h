/*
 * IR_gen.h
 *
 * Contains header functions for building Intermediate Representation of 
 * parse tree code
 *
 * Yondon Fu and Matt McFarland
 * Compilers - CS 57 (16W)
 */

#ifndef _IR_GEN_H
#define _IR_GEN_H

typedef enum {
	/* arithmatic operations */
	ADD_Q,
	SUB_Q,
	MUL_Q,
	DIV_Q,
	MOD_Q,
	ASSIGN_Q, 	

	/* comparison operations */
	LT_Q,
	GT_Q,
	LTE_Q,
	GTE_Q,
	NE_Q,
	EQ_Q,
	LDR_Q,
	STR_Q,

	/* jump operations */
	IFFALSE_Q, 	
	GOTO_Q,

	LABEL_Q,
	PRINT_Q,
	READ_Q,

	/* function operations */
	PROLOG_Q,
	EPILOG_Q,
	PRECALL_Q,
	POSTRET_Q
} quad_op;

/*
 * Quad structure
 *
 * op -> enumerated
 * dest -> where to store result
 * arg1 / arg2 -> can be registers, numbers or labels
 */
typedef struct quad {
	int op;
	int dest;
	int arg1;
	int arg2;
} quad;

/**
 * Traverses AST to generate code
 * Should return array of quads. Dynamically sizing array or LL?
 */
void CG(ast_node root);

/*
 * returns label of form "L_N[#]_[NODE TYPE]"
 * should be free'd after done using
 */
char * NewLabel(ast_node root);

/*
 * calls make_label() on root and all of root's children
 */
void print_label(ast_node root);

/*
 * NewTemp() 
 *
 * returns a new temp
 */


#endif 	// _IR_GEN_H