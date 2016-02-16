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

/*
 * given a quad, print that quad's code to the ys_file
 */
void print_code(quad * to_translate, FILE * ys_file_ptr);

/*
 * returns argument string to put in y86 assembly?
 */
char * handle_quad_arg(quad_arg *);

/*
 * Given that a variable can be a temp, local, parameter or global,
 * find the offset of the framepointer
 */
int get_variable_address_off_fp(quad_arg * var);

int get_global_address(quad_arg * global_var);




#endif 	// _TARGET_CODE_H