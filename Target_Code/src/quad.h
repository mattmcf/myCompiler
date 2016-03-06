/*
 * quad.h
 *
 * defines quad structure and types needed for it
 * Yondon Fu and Matt McFarland - Delights (CS57 - 16W)
 */

#ifndef _QUAD_H
#define _QUAD_H

#include "types.h" 	// for val-name pair struct
#include "temp_list.h"
#include "symtab.h"

#define QUAD_ARG_NUM 3   // MAGIC NUMBER !?!?! 

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
	INDEX_Q, 	// evaluate array index

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
	{INDEX_Q, "evaluate array index"},

	/* label generator */
	{LABEL_Q, "make label"},
  {0, NULL}
};

#define QUAD_INDEX(X)    ( (X) - ADD_Q)
#define QUAD_NAME(X)     ( quad_op_table[ QUAD_INDEX((X)) ].name)

typedef enum quad_arg_discriminant {
  NULL_ARG,   
  INT_LITERAL_Q_ARG,
  TEMP_VAR_Q_ARG,
  SYMBOL_VAR_Q_ARG,
  SYMBOL_ARR_Q_ARG,
  SYMBOL_FUNC_Q_ARG,
  LABEL_Q_ARG,
  RETURN_Q_ARG 				// filler to build continuity
} quad_arg_discriminant;

/*
 * at most, only one field is non-NULL
 */
typedef struct quad_arg {
  quad_arg_discriminant type;

  int int_literal; 			// holds array offset if SYMBOL_ARR_Q_ARG
  //struct quad_arg * index; 	// used in arrays indexed by expressions

  temp_var * temp;			// temp symbols and temps that index into array
  char * label;   			// for variable ID's, function ID's and Label ID's
  symnode_t * symnode; 		// function node for function labels
} quad_arg;

/*
 * Quad structure
 *
 * op -> enumerated
 * dest -> where to store result
 * arg1 / arg2 -> can be registers, numbers or labels
 */
typedef struct quad {
  int number;
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

#endif // _QUAD_H