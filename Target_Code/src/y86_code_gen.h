/*
 * target_code.h
 *
 * header file for target code generation functions
 * Yondon Fu and Matt McFarland - Delights (16W - CS57)
 */

#ifndef _TARGET_CODE_H
#define _TARGET_CODE_H

#include "quad.h" 		// for quad struct
#include <stdio.h>

#define STK_TOP 0x0000FFFF

/*
 * given a quad, print that quad's code to the ys_file
 */
void print_code(quad * to_translate, FILE * ys_file_ptr);

/*
 * returns argument string to put in y86 assembly?
 */
char * handle_quad_arg(quad_arg * arg);

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
 */
void set_variable_memory_locations(symboltable_t * symtab, int offset_to_set);

/*
 * called ONCE on the function scope table and then it explores down and sets variables
 */
void set_fp_offsets(symhashtable_t * symhash, int seen_locals, int seen_params);

#endif 	// _TARGET_CODE_H