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

	PRINT_Q,
	READ_Q,

	/* function operations */
	PROLOG_Q,
	EPILOG_Q,
	PRECALL_Q,
	POSTRET_Q,

	/* constant creation operations */
	STRING_Q,
	INT_LITERAL_Q
} quad_op;

/*
 * simplest temporary variable structure
 * each temporary will be given out once (so it's read once only)
 */
typedef struct temp_list {
	int count;
} temp_list;

/*
 * temporary variable structure
 */
typedef	struct temp_var {
	int id;
} temp_var;


/*
 * at most, only one field is non-NULL
 */
typedef struct quad_arg {
	int int_literal;
	temp_var * temp;
	char * label; 	// for variable ID's, function ID's and Label ID's
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
	quad_arg a1;
	quad_arg a2;
	quad_arg a3;
} quad;

/*
 * dynamically sized array of quads
 */
typedef struct quad_arr {
	quad * arr;
	int size; 			// max size
	int count;			// number of current entries
} quad_arr;

/*
 * Traverses AST to generate code
 * Should return array of quads. Dynamically sizing array or LL?
 */
void CG(ast_node root);

/*
 * returns label of form "L_N[#]_[NODE TYPE]"
 * should be free'd after done using
 */
char * new_label(ast_node root);

/*
 * calls make_label() on root and all of root's children
 */
void print_label(ast_node root);


/*
 * init_temp_list()
 *
 * returns NULL on failure
 */
temp_list * init_temp_list();

/*
 * new_temp()
 *
 * returns a new temp or NULL on failure
 */
 temp_var * new_temp(temp_list * lst);

/*
 * destroys a temporary variable struct
 */
void destroy_temp_var(temp_var * v);

/*
 * destroy_temp_list()
 *
 * destroys a temp_list
 */
void destroy_temp_list(temp_list * lst);

/*
 * initializes quad structure. GenQuad will fail until this is called.
 */
quad_arr * init_quad_list(quad_arr * lst);

/*
 * gen_quad -- adds quad to quad list 
 *
 * Looks for global quad_arr * called "quad_list" in main.c file
 *
 * Unused arguments are uninitialized quads
 */
void gen_quad(quad_op, quad_arg a1, quad_arg a2, quad_arg a3);

#endif 	// _IR_GEN_H