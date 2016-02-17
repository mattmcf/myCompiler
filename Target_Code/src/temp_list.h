/* temp_list.h
 * header file for temporary variable structs and functions
 *
 * Students: Yondon Fu and Matt McFarland - Delights (CS57 16W)
 * 
 */

#ifndef _TEMP_LIST_H
#define _TEMP_LIST_H

#include "stdlib.h"
#include "ast.h"
#include "types.h"
//#include "symtab.h" 		// double included

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
typedef struct temp_var {
  int id;
  void * temp_symnode; 	// because compiler issue
} temp_var;

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
 temp_var * new_temp(ast_node root, type_specifier_t type);

/*
 * destroys a temporary variable struct
 */
void destroy_temp_var(temp_var * v);

/*
 * makes a temp name on heap, must be freed later
 */
char * make_temp_name(int id);

/*
 * destroy_temp_list()
 *
 * destroys a temp_list
 */
void destroy_temp_list(temp_list * lst);

#endif // _TEMP_LIST_H