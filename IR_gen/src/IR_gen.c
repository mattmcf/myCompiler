
#include <stdlib.h> 		// for NULL
#include <stdio.h>          // for sprintf
#include <assert.h>        

#include "ast.h"
#include "types.h"
#include "toktypes.h"
#include "IR_gen.h"

#define MAX_LABEL_LENGTH 100 	// should be enough?

extern temp_list * temps_list;
extern quad_arr * quad_list;

#define INIT_QUAD_LIST_SIZE 200

void CG(ast_node root) {
  if (root != NULL) {
    ast_node child = root->left_child;

    while (child != NULL) {
      // Generate code for child
      switch (child->node_type) {
        case EXPRESSION_STMT_N:
          break;
        case IF_STMT_N:
          break;
        case IF_ELSE_STMT_N:
          break;
        case FOR_STMT_N:
          break;
        case WHILE_N:
          break;

        case OP_PLUS_N:
          // new temp1 for result
          // new temp2 = CG(arg1)
          // new temp3 = CG(arg2)
          // GenQuad(addl, temp1, temp2, temp3);
          break;

        case OP_MINUS_N:
          // new temp1 for result
          // GenQuad(addl, temp1, arg1, arg2);
          // return temp1
          break;

        case OP_NEG_N:
        case OP_TIMES_N:
        case OP_DIVIDE_N:
        case OP_MOD_N:
        case OP_LT_N:
        case OP_GT_N:
        case OP_GTE_N:
        case OP_LTE_N:
        case OP_EQ_N:
        case OP_NE_N:
        case OP_AND_N:
        case OP_OR_N:
        case OP_INC_N:
        case OP_DECR_N:

        case OP_ASSIGN_N:
          // temp_var * t = new_temp
          // get new temp
          // new_temp = CG(right hand side)
          // store temp at destination location (arg1)
          break;

        default:
          break;

        // Logical operations? Arrays?
      }

      // CG(child);
      child = child->right_sibling;
    }

    // Generate code for root
  }
}

/*
 * returns label of form "L_N[#]_[NODE TYPE]"
 * should be free'd after done using
 */
char * new_label(ast_node root) {
  if (!root)
    return NULL;

  char * label = (char *)calloc(MAX_LABEL_LENGTH,sizeof(char));
  assert(label);

  sprintf(label,"L_N%d_%s",root->id,NODE_NAME(root->node_type));

  assert(label);
  return label;
}


void print_label(ast_node root) {
  if (!root)
    return;

  char * label = new_label(root);
  printf("%s\n",label);
  free(label);

  for (ast_node child = root->left_child; child != NULL; child = child->right_sibling)
    new_label(child);
}



temp_list * init_temp_list() {
  temp_list * lst = (temp_list *)malloc(sizeof(temp_list));
  assert(lst);
  lst->count = 0;
  return lst;
}



temp_var * new_temp(temp_list * lst) {
  if (!lst)
    return NULL;

  temp_var * new_var = (temp_var *)malloc(sizeof(temp_var));
  assert(new_var);
  new_var->id = lst->count++;          // give unique id

  return new_var;
}


void destroy_temp_var(temp_var * v) {
  if (v != NULL)
    free(v);
}



void destroy_temp_list(temp_list * lst){
  if (lst != NULL)
    free(lst);
}


quad_arr * init_quad_list() {

  if(!quad_list) {
    quad_list = (quad_arr *)malloc(sizeof(quad) * INIT_QUAD_LIST_SIZE);
    assert(quad_list);
    quad_list->size = 0;
    quad_list->count = 0;
  }

  return quad_list;
}

/*
 * gen_quad -- adds quad to quad list 
 *
 * Looks for global quad_arr * called "quad_list" in main.c file
 *
 * Unused arguments are uninitialized quads
 *
 * returns 1 on failure
 */
int gen_quad(quad_op operation, quad_arg a1, quad_arg a2, quad_arg a3) {

  if (!quad_list)
    return 1;

  quad_list->arr[quad_list->count].op = operation;
  quad_list->arr[quad_list->count].args[0] = a1;
  quad_list->arr[quad_list->count].args[1] = a2;
  quad_list->arr[quad_list->count].args[2] = a3;

  /* double array size if full */
  if (++quad_list->count == quad_list->size) {
    printf("resizing quad_list to %d\n", 2 * quad_list->size);
    quad_list->size *= 2;
    quad_list->arr = realloc(quad_list->arr,sizeof(quad) * quad_list->size);
    assert(quad_list->arr);
  }

  return 0;
}

/*
 * looks for and prints "quad_list" in main.c
 */
void print_quad_list() {
  if (quad_list != NULL) {
    for (int i = 0; i < quad_list->count; i++) {
      print_quad(quad_list->arr[i]);
    }
  } else {
    fprintf(stderr,"cannot print quad list because list is null\n");
  }
}

void print_quad(quad q) {

  /* print quad operation */
  printf("(%s,",QUAD_NAME(q.op));

  /* print three arguments */
  quad_arg arg;
  for (int i = 0; i < QUAD_ARG_NUM; i++) {

    switch(q.args[i].type) {

      case INT_LITERAL_Q_ARG:
        printf("constant: %d",q.args[i].int_literal);
        break;

      case TEMP_VAR_Q_ARG:
        printf("temp: %d",q.args[i].temp->id);
        break;

      case LABEL_Q_ARG:
        printf("label: %s",q.args[i].label);
        break;

      default:  // null arg case
        printf(" - ");
        break;
    }

    if (i < QUAD_ARG_NUM - 1)
      printf(", ");
  }

  printf(")\n");
}

/*
 * looks for and frees "quad_list" global quad arr in main.c file 
 */
void destroy_quad_list() {
  if (quad_list != NULL)
    free(quad_list);
}

