
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

//#include "types.h"	// for val_name_pair struct
#include "temp_list.h"
#include "quad.h"

#define PASS_ARR_POINTER -1     // HACKY but it works -- needed by y86_code_gen

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