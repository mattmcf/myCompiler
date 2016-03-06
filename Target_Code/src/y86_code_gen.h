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
 * register enumerations and strings
 */
typedef enum {
	EAX_R,
	ECX_R,
	EDX_R,
	EBX_R,
	ESP_R,
	EBP_R,
	ESI_R,
	EDI_R
} my_register_t;

static val_name_pair reg_table[] = {
	{EAX_R, "%eax"},
	{ECX_R, "%ecx"},
	{EDX_R, "%edx"},
	{EBX_R, "%ebx"},
	{ESP_R, "%esp"},
	{EBP_R, "%ebp"},
	{ESI_R, "%esi"},
	{EDI_R, "%edi"}
};

#define REGISTER_INDEX(X) ( (X) - EAX_R )
#define REGISTER_STR(X) ( reg_table[ TYPE_INDEX((X)) ].name)

/*
 * creates ys file from global quad_list
 */
int create_ys(char * file_name);

/*
 * given a quad, print that quad's code to the ys_file
 */
void print_code(quad * to_translate, FILE * ys_file_ptr);

//char * load_arr_ptr(quad_arg * arr);
int get_source_value(FILE * fp, quad_arg * src, my_register_t dest);
int get_dest_value(FILE * fp, my_register_t src, quad_arg * dest);

/*
 * 	chooses irmovl, rrmovl, or mrmovl depending on source 
 */
//char * get_move_type(quad_arg * src); 

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
 * adds a STRING_Q to the ys file -- called after all executable quads are written
 */
void translate_string(FILE * ys_file_ptr, quad * string_to_add);

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