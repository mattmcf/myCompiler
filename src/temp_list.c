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
#define INIT_TEMP_SIZE 15

// initialize global list
temp_list * init_temp_list() {
  temp_list * t_lst = (temp_list *)malloc(sizeof(temp_list));
  assert(t_lst);

  t_lst->list = (temp_var **)calloc(INIT_TEMP_SIZE,sizeof(temp_var *));
  assert(t_lst->list);

  t_lst->size = INIT_TEMP_SIZE;
  t_lst->count = 0;
  return t_lst;
}

// get new temp
temp_var * new_temp(ast_node root) {
  if (!root)
    return NULL;

  // get root's scope table temp list
  temp_list * t_list = ((symhashtable_t *)root->scope_table)->t_list;

  // get a new temp from the list
  temp_var * new_var = (temp_var *)calloc(1,sizeof(temp_var));
  assert(new_var);

  // give unique id
  new_var->id = t_list->count;      

  // make new name -- not yet
  char * name = make_temp_name(new_var->id);

  // put new temp in list
  t_list->list[t_list->count] = new_var;

  // if list is full, expand! 
  t_list->count++;
  if (t_list->count == t_list->size) {
    t_list->size *= 2;
    t_list->list = realloc(t_list->list, t_list->size * sizeof(temp_var *));
    assert(t_list->list);
  }

  // add that new temp to local table under the name
  symnode_t * new_node = insert_into_symhashtable(root->scope_table,name, NULL);

  // attach temp to created symnode
  new_var->temp_symnode = new_node;

  // set node type
  set_node_type(new_node, VAR_SYM);

  // set node characteristics
  var_symbol node_model;
  node_model.name = name;
  node_model.type = INT_TS;
  node_model.modifier = SINGLE_DT;
  node_model.specie = TEMP_VAR;
  node_model.byte_size = TYPE_SIZE(INT_TS);

  // save to symbol table 
  set_node_var(new_node, &node_model);

  // return newly created temp
  return new_var;  
}

// void add_temps_to_scopes(symhashtable_t * symhash) {
//   if (!symhash)
//     return;

//   var_symbol node_model;
//   // node_model.name = name;  // set name in for loop
//   node_model.type = INT_TS;
//   node_model.modifier = SINGLE_DT;
//   node_model.specie = TEMP_VAR;

//   symnode_t * new_node;
//   for (int i = 0; i < symhash->t_list->count; i++) {
//     temp_var * v = symhash->t_list->list[i];
//     node_model.name = v->name;
//     symnode_t * new_node = insert_into_symhashtable(symhashtab,v->name, NULL);
//     set_node_type(VAR_SYM);
//     set_node_var(new_node, &node_model);
//   }

// }

// symnode_t * create_temp_symnode(temp_var * v) {
//   if (!v)
//     return NULL;

//   symnode_t * new_node = insert_into_symhashtable(symhashtab,name, NULL);
// }

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
  if (lst != NULL) {
    for (int i = 0; i < lst->count; i++ ) {
      destroy_temp_var(lst->list[i]);
    }
    free(lst);
  }
}