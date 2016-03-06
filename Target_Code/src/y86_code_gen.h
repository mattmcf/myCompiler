/*
 * target_code.h
 *
 * header file for target code generation functions
 * Yondon Fu and Matt McFarland - Delights (16W - CS57)
 */

#ifndef _TARGET_CODE_H
#define _TARGET_CODE_H

#include "quad.h" 		// for quad struct
#include "types.h"		// for val-name pair struct
#include <stdio.h> 		// for FILE *

/* Enum for conditions types */
typedef enum {
	LT_C,
	GT_C,
	LTE_C,
	GTE_C,
	NE_C,
	EQ_C,
	NULL_C
} condition_type;


/* 
 * type_specifier is for symbol.type
 */
// typedef enum {
//   NULL_TS,
//   INT_TS,       // int literals
//   VOID_TS,      // void returns
//   FUNC_TS       // not a type specifier -> used for symbols
// } type_specifier_t;

//  Table for type specifiers 
// static val_name_pair type_table[] = {
//   {NULL_TS, "NO DATA TYPE"},
//   {INT_TS, "INT"},
//   {VOID_TS, "VOID"},
//   {FUNC_TS, "FUNC"},
//   {0, NULL}
// };

/*
 * register enumerations
 */
// typedef enum {
// 	EAX_R,
// 	EBX_R,
	
// }

/*
 * creates ys file from global quad_list
 */
int create_ys(char * file_name);

/*
 * given a quad, print that quad's code to the ys_file
 */
void print_code(quad * to_translate, FILE * ys_file_ptr);

char * load_arr_ptr(quad_arg * arr);
char * get_source_value(quad_arg * src);
char * get_dest_value(quad_arg * dest); 

/*
 * 	chooses irmovl, rrmovl, or mrmovl depending on source 
 */
char * get_move_type(quad_arg * src); 

/*
 * returns argument string to put in y86 assembly? -- depreciated
 */
// char * handle_quad_arg(quad_arg * arg);

/*
 * generic subtraction operation for comparisons 
 */
void comp_sub(quad * to_translate, FILE * ys_file_ptr);

/*
 * Given that a variable can be a temp, local, parameter or global,
 * find the offset of the framepointer
 */
int get_variable_address_off_fp(quad_arg * var);

int get_global_address(quad_arg * global_var);


/*
 * before generating code, set all your frame pointer offsets for variables
 *
 * call this on root with `set_fp_offsets(root,0);`
 *
 * returns address of stack start
 */
int set_variable_memory_locations(symboltable_t * symtab);

/*
 * called ONCE on the function scope table and then it explores down and sets variables
 * 
 * returns lowest offset seen in function locals / temps
 */
int set_fp_offsets(symhashtable_t * symhash, int local_bytes, int param_bytes);

/*
 * prints comment line in ys
 */
void print_nop_comment(FILE * ys_ptr, char * msg, int id);

#endif 	// _TARGET_CODE_H