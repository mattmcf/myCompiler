
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

#define INIT_QUAD_LIST_SIZE 10

temp_var * CG(ast_node root) {
  temp_var * to_return;

  if (root != NULL) {
    switch (root->node_type) {
      // Switch on node types to handle root
      case IF_STMT_N:
        // new temp t1 = new_temp()
        // new label L_FI = new_label()
        // t1 = CG(root->left_child)
        // GenQuad(IFFALSE_Q, t1, L_FI, -)
        // CG(root->left_child->right_sibling)
        // new label L_FI = new_label()
        // GenQuad(LABEL_Q, L_FI, -, -)
        break;

      case IF_ELSE_STMT_N:
        // new temp t1 = new_temp()
        // new label L_ELSE = new_label()
        // t1 = CG(root->left_child)
        // GenQuad(IFFALSE_Q, t1, L_ELSE, -)
        // CG(root->left_child->right_sibling)
        // new label L_FI = new_label()
        // GenQuad(GOTO_Q, L_FI, -, -)
        // GenQuad(LABEL_Q, L_ELSE, -, -)
        // CG(root->left_child->right_sibling->right_sibling)
        // GenQuad(LABEL_Q, L_FI, -, -)
        break;

      case FOR_STMT_N:
        // new label L_FOR_TEST = new_label()
        // new label L_FOR_EXIT = new_label()
        // CG(root->left_child) // FOR HEADER INIT
        // GenQuad(LABEL_Q, L_FOR_TEST, -, -)
        // new temp t1 = new_temp()
        // t1 = CG(root->left_child->right_sibling) // FOR HEADER TEST
        // GenQuad(IFFALSE_Q, t1, L_FOR_EXIT, -)
        // CG(root->left_child->right_sibling->right_sibling->right_sibling) // FOR BODY
        // CG(root->left_child->right_sibling->right_sibling) // FOR HEADER INC or DEC
        // GenQuad(GOTO_Q, L_FOR_TEST, -, -)
        // GenQuad(LABEL_Q, L_FOR_EXIT, -, -)
        break;

      case WHILE_N:
        // new label L_WHILE_TEST = new_label()
        // new label L_WHILE_EXIT = new_label()
        // GenQuad(LABEL_Q, L_WHILE_TEST, -, -)
        // new temp t1 = new_temp()
        // t1 = CG(root->left_child)
        // GenQuad(IFFALSE_Q, t1, L_WHILE_EXIT, -)
        // CG(root->left_child->right_sibling)
        // GenQuad(GOTO_Q, L_WHILE_TEST, -, -)
        // GenQuad(LABEL_Q, L_WHILE_EXIT, -, -)
        break;

      case OP_ASSIGN_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // t2 = CG(root->left_child->right_sibling)
        // t3 = CG(root->left_child)
        // GenQuad(ASSIGN_Q, t1, t2)
        // GenQuad(ASSIGN_Q, t3, t1)
        // return t3
        {
          printf("Found assign\n");

          temp_var * t1 = CG(root->left_child->right_sibling);
          temp_var * t2 = CG(root->left_child);
          temp_var * t3 = new_temp(temps_list);

          // quad_arg * arg1 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg1->type = TEMP_VAR_Q_ARG;
          quad_arg * arg1 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg1->temp = t1;

          // quad_arg * arg2 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg2->type = TEMP_VAR_Q_ARG;
          quad_arg * arg2 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg2->temp = t2;

          // quad_arg * arg3 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg3->type = TEMP_VAR_Q_ARG;
          quad_arg * arg3 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg3->temp = t3;

          gen_quad(ASSIGN_Q, arg3, arg1, NULL);
          gen_quad(ASSIGN_Q, arg2, arg3, NULL);

          t3->int_literal = t1->int_literal;

          to_return = t3;

          return to_return;
        }

      case OP_PLUS_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // t2 = CG(root->left_child->right_sibling)
        // t3 = CG(root->left_child)
        // GenQuad(ADD_Q, t1, t2, t3);
        // return t1
        {
          printf("Found op plus\n");

          temp_var * t1 = CG(root->left_child->right_sibling);
          temp_var * t2 = CG(root->left_child);
          temp_var * t3 = new_temp(temps_list);

          // quad_arg * arg1 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg1->type = TEMP_VAR_Q_ARG;
          quad_arg * arg1 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg1->temp = t1;

          // quad_arg * arg2 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg2->type = TEMP_VAR_Q_ARG;
          quad_arg * arg2 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg2->temp = t2;

          // quad_arg * arg3 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg3->type = TEMP_VAR_Q_ARG;
          quad_arg * arg3 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg3->temp = t3;

          gen_quad(ADD_Q, arg3, arg1, arg2);

          t3->int_literal = t1->int_literal + t2->int_literal;

          to_return = t3;

          return to_return;
        }

      case OP_MINUS_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // GenQuad(SUB_Q, t1, t2, t3);
        // return t1
        {
          printf("Found op minus\n");

          temp_var * t1 = CG(root->left_child->right_sibling);
          temp_var * t2 = CG(root->left_child);
          temp_var * t3 = new_temp(temps_list);

          // quad_arg * arg1 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg1->type = TEMP_VAR_Q_ARG;
          quad_arg * arg1 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg1->temp = t1;

          // quad_arg * arg2 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg2->type = TEMP_VAR_Q_ARG;
          quad_arg * arg2 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg2->temp = t2;

          // quad_arg * arg3 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg3->type = TEMP_VAR_Q_ARG;
          quad_arg * arg3 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg3->temp = t3;

          gen_quad(SUB_Q, arg3, arg1, arg2);

          t3->int_literal = t1->int_literal - t2->int_literal;

          to_return = t3;

          return to_return;
        }

      case OP_TIMES_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // t2 = CG(root->left_child->right_sibling)
        // t3 = CG(root->left_child)
        // GenQuad(MUL_Q, t1, t2, t3);
        // return t1
        {
          printf("Found op mult\n");

          temp_var * t1 = CG(root->left_child->right_sibling);
          temp_var * t2 = CG(root->left_child);
          temp_var * t3 = new_temp(temps_list);

          // quad_arg * arg1 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg1->type = TEMP_VAR_Q_ARG;
          quad_arg * arg1 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg1->temp = t1;

          // quad_arg * arg2 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg2->type = TEMP_VAR_Q_ARG;
          quad_arg * arg2 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg2->temp = t2;

          // quad_arg * arg3 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg3->type = TEMP_VAR_Q_ARG;
          quad_arg * arg3 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg3->temp = t3;

          gen_quad(MUL_Q, arg3, arg1, arg2);

          t3->int_literal = t1->int_literal * t2->int_literal;

          to_return = t3;

          return to_return;
        }

      case OP_DIVIDE_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // t2 = CG(root->left_child->right_sibling)
        // t3 = CG(root->left_child)
        // GenQuad(DIV_Q, t1, t2, t3);
        // return t1
        {
          printf("Found op divide\n");

          temp_var * t1 = CG(root->left_child->right_sibling);
          temp_var * t2 = CG(root->left_child);
          temp_var * t3 = new_temp(temps_list);

          // quad_arg * arg1 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg1->type = TEMP_VAR_Q_ARG;
          quad_arg * arg1 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg1->temp = t1;

          // quad_arg * arg2 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg2->type = TEMP_VAR_Q_ARG;
          quad_arg * arg2 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg2->temp = t2;

          // quad_arg * arg3 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg3->type = TEMP_VAR_Q_ARG;
          quad_arg * arg3 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg3->temp = t3;

          gen_quad(DIV_Q, arg3, arg1, arg2);

          t3->int_literal = t1->int_literal / t2->int_literal;

          to_return = t3;

          return to_return;
        }

      case OP_MOD_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // t2 = CG(root->left_child->right_sibling)
        // t3 = CG(root->left_child)
        // GenQuad(MOD_Q, t1, t2, t3);
        // return t1
        {
          printf("Found op mod\n");

          temp_var * t1 = CG(root->left_child->right_sibling);
          temp_var * t2 = CG(root->left_child);
          temp_var * t3 = new_temp(temps_list);

          // quad_arg * arg1 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg1->type = TEMP_VAR_Q_ARG;
          quad_arg * arg1 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg1->temp = t1;

          // quad_arg * arg2 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg2->type = TEMP_VAR_Q_ARG;
          quad_arg * arg2 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg2->temp = t2;

          // quad_arg * arg3 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg3->type = TEMP_VAR_Q_ARG;
          quad_arg * arg3 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg3->temp = t3;

          gen_quad(MOD_Q, arg3, arg1, arg2);

          t3->int_literal = t1->int_literal % t2->int_literal;

          to_return = t3;

          return to_return;
        }

      case OP_INC_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // t1 = CG(root->left_child)
        // GenQuad(ADD_Q, t2, t1, 1)
        // return t2
        {
          printf("Found op increment\n");

          temp_var * t1 = CG(root->left_child);
          temp_var * t2 = new_temp(temps_list);

          // quad_arg * arg1 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg1->type = TEMP_VAR_Q_ARG;
          quad_arg * arg1 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg1->temp = t1;

          // quad_arg * arg2 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg2->type = TEMP_VAR_Q_ARG;
          quad_arg * arg2 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg2->temp = t2;

          // quad_arg * arg3 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg3->type = INT_LITERAL_Q_ARG;
          quad_arg * arg3 = create_quad_arg(INT_LITERAL_Q_ARG);
          arg3->int_literal = 1;

          gen_quad(ADD_Q, arg2, arg1, arg3);

          break;
        }

      case OP_DECR_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // t1 = CG(root->left_child)
        // GenQuad(SUB_Q, t2, t1, 1)
        // return t2
        {
          printf("Found op decrement\n");

          temp_var * t1 = CG(root->left_child);
          temp_var * t2 = new_temp(temps_list);

          // quad_arg * arg1 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg1->type = TEMP_VAR_Q_ARG;
          quad_arg * arg1 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg1->temp = t1;

          // quad_arg * arg2 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg2->type = TEMP_VAR_Q_ARG;
          quad_arg * arg2 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg2->temp = t2;

          // quad_arg * arg3 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg3->type = INT_LITERAL_Q_ARG;
          quad_arg * arg3 = create_quad_arg(INT_LITERAL_Q_ARG);
          arg3->int_literal = 1;

          gen_quad(SUB_Q, arg2, arg1, arg3);
          
          break;
        }

      case OP_EQ_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // t1 = CG(root->left_child)
        // t2 = CG(root->left_child->right_sibling)
        // GenQuad(EQ_Q, t3, t1, t2)
        // return t3
        {
     
          printf("Found op equals\n");

          temp_var * t1 = CG(root->left_child->right_sibling);
          temp_var * t2 = CG(root->left_child);
          temp_var * t3 = new_temp(temps_list);

          // quad_arg * arg1 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg1->type = TEMP_VAR_Q_ARG;
          quad_arg * arg1 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg1->temp = t1;

          // quad_arg * arg2 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg2->type = TEMP_VAR_Q_ARG;
          quad_arg * arg2 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg2->temp = t2;

          // quad_arg * arg3 = (quad_arg *)malloc(sizeof(quad_arg));
          // arg3->type = TEMP_VAR_Q_ARG;
          quad_arg * arg3 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg3->temp = t3;

          gen_quad(EQ_Q, arg3, arg1, arg2);

          /* ? */
          if (t1->int_literal == t2->int_literal) {
            t3->int_literal = 1;
          } else {
            t3->int_literal = 0;
          }

          to_return = t3;

          return to_return;
        }

      case OP_NE_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // t1 = CG(root->left_child)
        // t2 = CG(root->left_child->right_sibling)
        // GenQuad(NE_Q, t3, t1, t2)
        // return t3
        break;

      case OP_LT_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // t1 = CG(root->left_child)
        // t2 = CG(root->left_child->right_sibling)
        // GenQuad(LT_Q, t3, t1, t2)
        // return t3
        break;

      case OP_GT_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // t1 = CG(root->left_child)
        // t2 = CG(root->left_child->right_sibling)
        // GenQuad(GT_Q, t3, t1, t2)
        // return t3
        break;

      case OP_GTE_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // t1 = CG(root->left_child)
        // t2 = CG(root->left_child->right_sibling)
        // GenQuad(GTE_Q, t3, t1, t2)
        // return t3
        break;

      case OP_LTE_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // t1 = CG(root->left_child)
        // t2 = CG(root->left_child->right_sibling)
        // GenQuad(LTE_Q, t3, t1, t2)
        // return t3
        break;

      case OP_AND_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // new label L_FALSE = new_label()
        // new label L_DONE = new_label()
        // t1 = CG(root->left_child)
        // GenQuad(IFFALSE_Q, t1, L_FALSE, -)
        // t2 = CG(root->left_child->right_sibling)
        // GenQuad(IFFALSE_Q, t2, L_FALSE, -)
        // GenQuad(ASSIGN_Q, t3, 1, -)
        // GenQuad(GOTO_Q, L_DONE, -, -)
        // GenQuad(LABEL_Q, L_FALSE, -, -)
        // GenQuad(ASSIGN_Q, t3, 0, -)
        // GenQuad(LABEL_Q, L_DONE, -, -)
        // return t3
        break;

      case OP_OR_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // new label L_FALSE = new_label()
        // new label L_ALL_FALSE = new_label()
        // new label L_DONE = new_label()
        // t1 = CG(root->left_child)
        // GenQuad(IFFALSE_Q, t1, L_FALSE, -)
        // GenQuad(ASSIGN_Q, t2, 1, -) // t1 was true, so skip RHS expr
        // GenQuad(GOTO_Q, L_DONE, -, -) // Skip to finish
        // GenQuad(LABEL_Q, L_FALSE, -, -)
        // t3 = CG(root->left_child->right_sibling) // t1 was false, so evaluate RHS expr
        // GenQuad(IFFALSE_Q, t3, L_ALL_FALSE, -)
        // GenQuad(ASSIGN_Q, t2, 1, -) // t3 was True
        // GenQuad(GOTO_Q, L_DONE, -, -)
        // GenQuad(LABEL_Q, L_ALL_FALSE, -, -)
        // GenQuad(ASSIGN_Q, t2, 0, -) // t1 and t3 both false
        // GenQuad(LABEL_Q, L_DONE, -, -)
        // return t2
        break;

      case OP_NEG_N:
        // ??
        break;

      case CALL_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // t1 = CG(root->left_child->right_sibling) // Get argument list
        // for arg in t1:
        //   new temp t = new_temp()
        //   GenQuad(ASSIGN_Q, t, arg, -)
        // new temp t2 = CG(root->left_child) // Get function
        // GenQuad(PRECALL_Q, t2, -, -) // Specify function being called
        // GenQuad(POSTRET_Q, t2, -, -) // Specify function being called
        break;

      case PRINT_N:
        // new temp t1 = new_temp()
        // t1 = CG(root->left_child)
        // GenQuad(PRINT_Q, t1, -, -)
        break;

      case READ_N:
        // new temp t1 = new_temp()
        // t1 = CG(root->left_child)
        // GenQuad(READ_Q, t1, -, -)
        break;

      case ID_N:
        to_return = new_temp(temps_list);
        to_return->var_id = root->value_string;
        return to_return;

      case INT_LITERAL_N:
        to_return = new_temp(temps_list);
        to_return->int_literal = root->value_int;
        return to_return;

      default:
        {
          ast_node child = root->left_child;

          /* do we really want to call GC on children here? Or should this be rolled into the default case? */
          while (child != NULL) {
            to_return = CG(child);
            child = child->right_sibling;
          }

          break;
        }

    }

    
  }

  return to_return;
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

  sprintf(label,"L_N%d_%s", root->id, NODE_NAME(root->node_type));

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

/*
 * Temp List functions
 */

// initialize global list
temp_list * init_temp_list() {
  temp_list * lst = (temp_list *)malloc(sizeof(temp_list));
  assert(lst);
  lst->count = 0;
  return lst;
}

// get new temp
temp_var * new_temp(temp_list * lst) {
  if (!lst)
    return NULL;

  temp_var * new_var = (temp_var *)malloc(sizeof(temp_var));
  assert(new_var);
  new_var->id = lst->count++;          // give unique id

  return new_var;
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

/*
 * Functions to handle quad list and quad structs
 */

// initialize global quad list
quad_arr * init_quad_list() {

  if(!quad_list) {
    quad_list = (quad_arr *)malloc(sizeof(quad_arr));
    assert(quad_list);
    quad_list->arr = (quad **)malloc(sizeof(quad) * INIT_QUAD_LIST_SIZE);
    quad_list->size = 0;
    quad_list->count = 0;
  }

  return quad_list;
}

// create a quad arg struct of type
quad_arg * create_quad_arg(quad_arg_discriminant type) {
  quad_arg * new_arg = (quad_arg *)malloc(sizeof(quad_arg));
  assert(new_arg);
  new_arg->type = type;
  return new_arg;
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
int gen_quad(quad_op operation, quad_arg * a1, quad_arg * a2, quad_arg * a3) {

  if (!quad_list)
    return 1;

  quad_list->arr[quad_list->count] = (quad *)malloc(sizeof(quad));
  quad_list->arr[quad_list->count]->op = operation;
  quad_list->arr[quad_list->count]->args[0] = a1;
  quad_list->arr[quad_list->count]->args[1] = a2;
  quad_list->arr[quad_list->count]->args[2] = a3;

  /* double array size if full */
  if (++quad_list->count == quad_list->size) {
    printf("resizing quad_list to %d\n", 2 * quad_list->size);
    quad_list->size *= 2;
    quad_list->arr = realloc(quad_list->arr,sizeof(quad) * quad_list->size);
    assert(quad_list->arr);
  }

  return 0;
}

// prints global quad list
void print_quad_list() {
  if (quad_list != NULL) {
    for (int i = 0; i < quad_list->count; i++) {
      print_quad(quad_list->arr[i]);
    }
  } else {
    fprintf(stderr,"cannot print quad list because list is null\n");
  }
}

// print a quad
void print_quad(quad * q) {

  /* print quad operation */
  printf("(%s, ",QUAD_NAME(q->op));

  /* print three arguments */
  quad_arg arg;
  for (int i = 0; i < QUAD_ARG_NUM; i++) {
    if (q->args[i] == NULL) {
      printf(" - ");
    } else {
      switch(q->args[i]->type) {

      case INT_LITERAL_Q_ARG:
        printf("Constant: %d",q->args[i]->int_literal);
        break;

      case TEMP_VAR_Q_ARG:
        printf("Temp (ID: %d)",
          q->args[i]->temp->id);
        break;

      case LABEL_Q_ARG:
        printf("Label: %s",q->args[i]->label);
        break;

      default:  // null arg case
        printf(" - ");
        break;
      }
    } 

    if (i < QUAD_ARG_NUM - 1)
      printf(", ");
  }

  printf(")\n");
}

// destroys global quad list
void destroy_quad_list() {
  if (quad_list != NULL)
    free(quad_list);
}

