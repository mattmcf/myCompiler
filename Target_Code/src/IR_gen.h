
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

#include "types.h"	// for val_name_pair struct
#include "temp_list.h"

#define QUAD_ARG_NUM 3  /* MAGIC NUMBER !?!?! */

typedef enum {
	/* arithmetic operations */
	ADD_Q, // Add
	SUB_Q, // Subtract
	MUL_Q, // Multiply
	DIV_Q, // Divide
	MOD_Q, // Modulo
	INC_Q, // Increment
	DEC_Q, // Decrement
	
	NOT_Q, // Not - negation
	NEG_Q, // Unary minus aka negative

	ASSIGN_Q, // Assignment

	/* comparison operations */
	LT_Q, // Less than
	GT_Q, // Greater than
	LTE_Q, // Less than or equal to
	GTE_Q, // Greater than or equal to
	NE_Q, // Not equals
	EQ_Q, // Equals

	/* jump operations */
	IFFALSE_Q, // If false, jump to label
	GOTO_Q, // Go to label

	PRINT_Q, // Print
	READ_Q, // Read

	/* function operations */
	PROLOG_Q, // Function prologue
	EPILOG_Q, // Function epilogue
	PRECALL_Q, // Pre call
	POSTRET_Q, // Post return
	PARAM_Q, // Create function argument/parameter
	RET_Q, // Function return

	/* constant creation operations */
	STRING_Q, // Create string literal

	LABEL_Q // Create label
} quad_op;

static val_name_pair quad_op_table[] = {
	/* arithmatic operations */
	{ADD_Q, "add"},
	{SUB_Q, "sub"},
	{MUL_Q, "multiply"},
	{DIV_Q, "divide"},
	{MOD_Q, "mod divide"},
	{INC_Q, "increment"},
	{DEC_Q, "decrement"},

	{NOT_Q, "not"},
	{NEG_Q, "negative"},

	{ASSIGN_Q, "assign"}, 	

	/* comparison operations */
	{LT_Q, "less than"},
	{GT_Q, "greater than"},
	{LTE_Q, "less than or equal to"},
	{GTE_Q, "greater than or equal to"},
	{NE_Q, "not equal"},
	{EQ_Q, "equal to"},

	/* jump operations */
	{IFFALSE_Q, "if false"}, 	
	{GOTO_Q, "goto"},

	{PRINT_Q, "print"},
	{READ_Q, "read"},

	/* function operations */
	{PROLOG_Q, "prolog"},
	{EPILOG_Q, "epilog"},
	{PRECALL_Q, "precall"},
	{POSTRET_Q, "post return"},
	{PARAM_Q, "param"},
	{RET_Q, "return"},

	/* constant creation operations */
	{STRING_Q, "save string"},

	/* label generator */
	{LABEL_Q, "make label"},
  {0, NULL}
};

#define QUAD_INDEX(X)    ( (X) - ADD_Q)
#define QUAD_NAME(X)     ( quad_op_table[ QUAD_INDEX((X)) ].name)

/*
 * returns label of form "L_N[#]_[NODE TYPE]"
 * should be free'd after done using
 */
char * new_label(ast_node root, char * name);

/*
 * calls make_label() on root and all of root's children
 */
void print_label(ast_node root);

typedef enum quad_arg_discriminant {
  NULL_ARG,   
  INT_LITERAL_Q_ARG,
  TEMP_VAR_Q_ARG,
  SYMBOL_VAR_Q_ARG,
  SYMBOL_ARR_Q_ARG,
  LABEL_Q_ARG,
  RETURN_Q_ARG 				// filler to build continuity
} quad_arg_discriminant;

/*
 * at most, only one field is non-NULL
 */
typedef struct quad_arg {
  quad_arg_discriminant type;
  int int_literal; 			// holds array offset if SYMBOL_ARR_Q_ARG (0 for singles)
  temp_var * temp;
  char * label;   			// for variable ID's, function ID's and Label ID's
} quad_arg;

/*
 * Quad structure
 *
 * op -> enumerated
 * dest -> where to store result
 * arg1 / arg2 -> can be registers, numbers or labels
 */
typedef struct quad {
  quad_op op;
  quad_arg * args[QUAD_ARG_NUM];
} quad;

/*
 * dynamically sized array of quads
 */
typedef struct quad_arr {
  quad ** arr;
  int size;       // max size
  int count;      // number of current entries (points at first unused entry)
} quad_arr;

/*
 * Traverses AST to generate code
 * Should return array of quads. Dynamically sizing array or LL?
 */
quad_arg * CG(ast_node root);

quad_arg * CG_assign_op(ast_node root);
quad_arg * CG_math_op(ast_node root, quad_op op);

/*
 * returns label of form "L_N[#]_[NODE TYPE]"
 * should be free'd after done using
 */
char * new_label(ast_node root, char * name);

/*
 * calls make_label() on root and all of root's children
 */
void print_label(ast_node root);

/*
 * initializes global "quad_list" quad_arr structure. GenQuad will fail until this is called.
 */
quad_arr * init_quad_list();

/* 
 * returns quad_arg pointer to newly created quad_arg struct of type
 * must fill contents after creation
 */
quad_arg * create_quad_arg(quad_arg_discriminant type);

/*
 * gen_quad -- adds quad to quad list 
 *
 * Looks for global quad_arr * called "quad_list" in main.c file
 *
 * Unused arguments are uninitialized quads
 *
 * returns 1 on failure
 */
int gen_quad(quad_op, quad_arg * a1, quad_arg * a2, quad_arg * a3);

/*
 * looks for and prints "quad_list" in main.c
 */
void print_quad_list();

/*
 * prints quad
 */
void print_quad(quad * q);

/*
 * looks for and frees "quad_list" global quad arr in main.c file 
 */
void destroy_quad_list();

#endif 	// _IR_GEN_H