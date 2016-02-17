/* temp_list.c
 * temporary variable functions
 *
 * Students: Yondon Fu and Matt McFarland - Delights (CS57 16W)
 * 
 */

#include "temp_list.h"
#include "types.h"
#include "symtab.h"
#include "ast.h"
#include "stdio.h" 		// for sprintf
#include <assert.h>

#define MAX_TEMP_NAME_LENGTH 50

// initialize global list
temp_list * init_temp_list() {
  temp_list * lst = (temp_list *)malloc(sizeof(temp_list));
  assert(lst);
  lst->count = 0;
  return lst;
}

// get new temp
temp_var * new_temp(ast_node root, type_specifier_t type) {
  if (!root)
    return NULL;

  // get root's scope table temp list
  symhashtable_t * symhashtab = root->scope_table;
  temp_list * t_list = symhashtab->t_list;

  // get a new temp from the list
  temp_var * new_var = (temp_var *)calloc(1,sizeof(temp_var));
  assert(new_var);

  // give unique id
  new_var->id = t_list->count++;      

  // make new name
  char * name = make_temp_name(new_var->id);

  // calculate offset on local stack
  // int fp_offset = symhashtab->local_sp;
  // symhashtab->local_sp += TYPE_SIZE(type);

  // add that new temp to local table under the name
  symnode_t * new_node = insert_into_symhashtable(symhashtab,name);

  // attach temp to created symnode
  new_var->temp_symnode = new_node;

  // set node type
  set_node_type(new_node, VAR_SYM);

  // set node characteristics
  var_symbol node_model;
  node_model.name = name;
  node_model.type = type;
  node_model.modifier = SINGLE_DT;
  node_model.specie = TEMP_VAR;
  //node_model.offset_of_frame_pointer = fp_offset;

  // save to symbol table 
  set_node_var(new_node, &node_model);

  return new_var;
}


char * make_temp_name(int id) {
	char * str = (char *)calloc(MAX_TEMP_NAME_LENGTH, sizeof(char));
	assert(str);
	sprintf(str,"%d_temp",id);
	return str;
}

// destroy temp variable
void destroy_temp_var(temp_var * v) {
  if (v != NULL)
    free(v);
}

// destroy whole temp list
void destroy_temp_list(temp_list * lst){
  if (lst != NULL)
    free(lst);
}