
#include <stdlib.h> 		// for NULL
#include <stdio.h>          // for sprintf
#include <assert.h>        

#include "ast.h"
#include "types.h"
#include "toktypes.h"
#include "IR_gen.h"

#define INIT_QUAD_LIST_SIZE 10
#define PASS_ARR_POINTER -1     // HACKY but it works
#define MAX_LABEL_LENGTH 100 	  // should be enough?

extern quad_arr * quad_list;

quad_arg * CG(ast_node root) {
  quad_arg * to_return;

  if (root != NULL) {
    switch (root->node_type) {
      // Switch on node types to handle root
      case VAR_DECL_N:
        // Check if there is a RHS value
        if ( (root->left_child->mod == SINGLE_DT) && (root->left_child->right_sibling != NULL) ) {

          /* have to build assignment quads here */
          quad_arg * var_arg, * initialization_value;

          var_arg = create_quad_arg(SYMBOL_VAR_Q_ARG);
          var_arg->label = root->left_child->value_string;
          var_arg->symnode = look_up_scopes_to_find_symbol(root->left_child->scope_table, var_arg->label);

          initialization_value = CG(root->left_child->right_sibling);          

          gen_quad(ASSIGN_Q, var_arg, initialization_value, NULL);
        }
        break;

      case OP_ASSIGN_N:
        to_return = CG_assign_op(root);
        break;

      case OP_PLUS_N:
        to_return = CG_math_op(root, ADD_Q);
        break;

      case OP_MINUS_N:
        to_return = CG_math_op(root, SUB_Q);
        break;

      case OP_TIMES_N:
        to_return = CG_math_op(root, MUL_Q);
        break;

      case OP_DIVIDE_N:
        to_return = CG_math_op(root, DIV_Q);
        break;

      case OP_MOD_N:
        to_return = CG_math_op(root, MOD_Q);
        break;

      case OP_INC_N:
        to_return = CG_math_op(root, INC_Q);
        break;

      case OP_DECR_N:
        to_return = CG_math_op(root, DEC_Q);
        break;

      case OP_EQ_N:
        to_return = CG_math_op(root, EQ_Q);
        break;

      case OP_NE_N:
        to_return = CG_math_op(root, NE_Q);
        break;

      case OP_LT_N:
        to_return = CG_math_op(root, LT_Q);
        break;

      case OP_GT_N:
       to_return = CG_math_op(root, GT_Q);
       break;

      case OP_GTE_N:
        to_return = CG_math_op(root, GTE_Q);
        break;

      case OP_LTE_N:
        to_return = CG_math_op(root, LTE_Q);
        break;

      case IF_STMT_N:
        // new temp t1 = new_temp()
        // new label L_FI = new_label()
        // t1 = CG(root->left_child)
        // GenQuad(IFFALSE_Q, t1, L_FI, -)
        // CG(root->left_child->right_sibling)
        // new label L_FI = new_label()
        // GenQuad(LABEL_Q, L_FI, -, -)
        {
          quad_arg * arg1 = CG(root->left_child);

          char * label_fi = new_label(root, "FI");
          quad_arg * arg2 = create_quad_arg(LABEL_Q_ARG);
          arg2->label = label_fi;

          gen_quad(IFFALSE_Q, arg1, arg2, NULL);

          CG(root->left_child->right_sibling);

          gen_quad(LABEL_Q, arg2, NULL, NULL);

          break;
        }

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
        {
          quad_arg * arg1 = CG(root->left_child);
          
          char * label_else = new_label(root, "ELSE");
          quad_arg * else_arg = create_quad_arg(LABEL_Q_ARG);
          else_arg->label = label_else;

          char * label_fi = new_label(root, "FI");
          quad_arg * fi_arg = create_quad_arg(LABEL_Q_ARG);
          fi_arg->label = label_fi;

          gen_quad(IFFALSE_Q, arg1, else_arg, NULL);

          CG(root->left_child->right_sibling);

          gen_quad(GOTO_Q, fi_arg, NULL, NULL);

          gen_quad(LABEL_Q, else_arg, NULL, NULL);

          CG(root->left_child->right_sibling->right_sibling);

          gen_quad(LABEL_Q, fi_arg, NULL, NULL);

          break;
        }

      case FOR_STMT_N:
        {
          char * label_test = new_label(root, "FOR_TEST");
          quad_arg * test_arg = create_quad_arg(LABEL_Q_ARG);
          test_arg->label = label_test;

          char * label_exit = new_label(root, "FOR_EXIT");
          quad_arg * exit_arg = create_quad_arg(LABEL_Q_ARG);
          exit_arg->label = label_exit;

          CG(root->left_child);

          gen_quad(LABEL_Q, test_arg, NULL, NULL);

          quad_arg * arg1 = CG(root->left_child->right_sibling);
          gen_quad(IFFALSE_Q, arg1, exit_arg, NULL);
          CG(root->left_child->right_sibling->right_sibling->right_sibling);
          CG(root->left_child->right_sibling->right_sibling);

          gen_quad(GOTO_Q, test_arg, NULL, NULL);
          gen_quad(LABEL_Q, exit_arg, NULL, NULL);

          break;
        }

      case WHILE_N:
        {
          char * label_test = new_label(root, "WHILE_TEST");
          quad_arg * test_arg = create_quad_arg(LABEL_Q_ARG);
          test_arg->label = label_test;

          char * label_exit = new_label(root, "WHILE_EXIT");
          quad_arg * exit_arg = create_quad_arg(LABEL_Q_ARG);
          exit_arg->label = label_exit;

          gen_quad(LABEL_Q, test_arg, NULL, NULL);

          quad_arg * arg1 = CG(root->left_child);

          gen_quad(IFFALSE_Q, arg1, exit_arg, NULL);

          CG(root->left_child->right_sibling);

          gen_quad(GOTO_Q, test_arg, NULL, NULL);
          gen_quad(LABEL_Q, exit_arg, NULL, NULL);

          break;
        }

      case OP_AND_N:
        {
          char * label_false = new_label(root, "FALSE");
          quad_arg * false_arg = create_quad_arg(LABEL_Q_ARG);
          false_arg->label = label_false;

          char * label_done = new_label(root, "DONE");
          quad_arg * done_arg = create_quad_arg(LABEL_Q_ARG);
          done_arg->label = label_done;

          temp_var * t1 = new_temp(root);

          quad_arg * arg1 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg1->temp = t1;

          quad_arg * res_true = create_quad_arg(INT_LITERAL_Q_ARG);
          res_true->int_literal = 1;

          quad_arg * res_false = create_quad_arg(INT_LITERAL_Q_ARG);
          res_false->int_literal = 0;

          quad_arg * arg2 = CG(root->left_child);

          gen_quad(IFFALSE_Q, arg2, false_arg, NULL);

          quad_arg * arg3 = CG(root->left_child->right_sibling);

          gen_quad(IFFALSE_Q, arg3, false_arg, NULL);

          gen_quad(ASSIGN_Q, arg1, res_true, NULL);
          gen_quad(GOTO_Q, done_arg, NULL, NULL);

          gen_quad(LABEL_Q, false_arg, NULL, NULL);
          gen_quad(ASSIGN_Q, arg1, res_false, NULL);

          gen_quad(LABEL_Q, done_arg, NULL, NULL);

          to_return = arg1;

          break;
        }

      case OP_OR_N:
        {
          char * label_false = new_label(root, "FALSE");
          quad_arg * false_arg = create_quad_arg(LABEL_Q_ARG);
          false_arg->label = label_false;

          char * label_all_false = new_label(root, "ALL_FALSE");
          quad_arg * all_false_arg = create_quad_arg(LABEL_Q_ARG);
          all_false_arg->label = label_all_false;

          char * label_done = new_label(root, "DONE");
          quad_arg * done_arg = create_quad_arg(LABEL_Q_ARG);
          done_arg->label = label_done;

          quad_arg * res_true = create_quad_arg(INT_LITERAL_Q_ARG);
          res_true->int_literal = 1;

          quad_arg * res_false = create_quad_arg(INT_LITERAL_Q_ARG);
          res_false->int_literal = 0;

          temp_var * t1 = new_temp(root);
          quad_arg * arg1 = create_quad_arg(TEMP_VAR_Q_ARG);
          arg1->temp = t1;

          quad_arg * arg2 = CG(root->left_child);

          gen_quad(IFFALSE_Q, arg2, false_arg, NULL);
          gen_quad(ASSIGN_Q, arg1, res_true, NULL);
          gen_quad(GOTO_Q, done_arg, NULL, NULL);
          gen_quad(LABEL_Q, false_arg, NULL, NULL);

          quad_arg * arg3 = CG(root->left_child->right_sibling);

          gen_quad(IFFALSE_Q, arg3, all_false_arg, NULL);
          gen_quad(ASSIGN_Q, arg1, res_true, NULL);
          gen_quad(GOTO_Q, done_arg, NULL, NULL);
          gen_quad(LABEL_Q, all_false_arg, NULL, NULL);
          gen_quad(ASSIGN_Q, arg1, res_false, NULL);
          gen_quad(LABEL_Q, done_arg, NULL, NULL);

          to_return = arg1;
          break;
        }

      case OP_NOT_N:
        to_return = CG_math_op(root, NOT_Q);
        break;

      case OP_NEG_N:
        to_return = CG_math_op(root, NEG_Q);
        break;

      case FUNC_DECLARATION_N:
        {
          ast_node func_id = root->left_child->right_sibling;

          quad_arg * func_arg = CG(func_id);
          func_arg->symnode = look_up_scopes_to_find_symbol(root->scope_table, func_id->value_string);
          gen_quad(PROLOG_Q, func_arg, NULL, NULL);

          CG(root->left_child->right_sibling->right_sibling->right_sibling);

          char * epilog_label = new_label(root,"EPILOG");
          quad_arg * epilog_arg = create_quad_arg(LABEL_Q_ARG);
          epilog_arg->label = epilog_label;

          gen_quad(LABEL_Q, epilog_arg, NULL, NULL);
          gen_quad(EPILOG_Q, func_arg, NULL, NULL);

          break;
        }

      case RETURN_N:
        // get value to return to caller
        // move that into return location
        // jump to epilog
        {
          ast_node pf = root->parent_function;
          char * epilog_label = new_label(pf,"EPILOG");
          quad_arg * epilog_arg = create_quad_arg(LABEL_Q_ARG);
          epilog_arg->label = epilog_label;

          quad_arg * function_return = CG(root->left_child);
          gen_quad(RET_Q, function_return, NULL, NULL);
          gen_quad(GOTO_Q, epilog_arg, NULL, NULL);

          break;
        }


      case CALL_N:
        {
          if (root->left_child->right_sibling != NULL) {
            ast_node param = root->left_child->right_sibling->left_child;

            while (param != NULL) {
              quad_arg * param_arg = CG(param);
              gen_quad(PARAM_Q, param_arg, NULL, NULL);     // when encountering PARAM_Q, push onto stack

              param = param->right_sibling;
            }            
          }

          quad_arg * func_arg = CG(root->left_child);
          // quad_arg * func_arg = create_quad_arg(SYMBOL_FUNC_Q_ARG);
          // func_arg->symnode = root;
          // func_arg->label = root->left_child->value_string;
          gen_quad(PRECALL_Q, func_arg, NULL, NULL);
          gen_quad(POSTRET_Q, func_arg, NULL, NULL);

          // We should probably return a quad arg here that can be used
          // for assignment. But how do we get the return value?
          to_return = create_quad_arg(RETURN_Q_ARG);

          break;
        }

      case PRINT_N:
        {
          quad_arg * arg1 = CG(root->left_child);
          gen_quad(PRINT_Q, arg1, NULL, NULL);

          break;
        }

      case READ_N:
        {
          quad_arg * arg1 = CG(root->left_child);
          gen_quad(READ_Q, arg1, NULL, NULL);

          break;
        }

      case VAR_N:
        // check if accessing array or just a single variable
        {
          if (root->left_child->right_sibling == NULL && root->mod == SINGLE_DT) {
            to_return = create_quad_arg(SYMBOL_VAR_Q_ARG);
            to_return->label = root->left_child->value_string;
            to_return->symnode = look_up_scopes_to_find_symbol(root->scope_table, to_return->label);
          } else {
            to_return = create_quad_arg(SYMBOL_ARR_Q_ARG);
            to_return->label = root->left_child->value_string;
            to_return->symnode = look_up_scopes_to_find_symbol(root->scope_table, to_return->label);

            // check if accessing like an array
            if (root->left_child->right_sibling != NULL)
              to_return->int_literal = root->left_child->right_sibling->left_child->value_int;     // get offset into array  
            else
              to_return->int_literal = PASS_ARR_POINTER;       
          }
          break;
        }

      case ID_N:
        to_return = create_quad_arg(LABEL_Q_ARG);
        to_return->label = root->value_string;        
        break;

      case STRING_N:
        // save string in memory with label and incorporate jumps around string
        {
          char * end_label = new_label(root,"END_STRING");
          quad_arg * end_label_arg = create_quad_arg(LABEL_Q_ARG);
          end_label_arg->label = end_label;

          char * ascii_label = new_label(root,"DEF_STRING");
          quad_arg * ascii_arg = create_quad_arg(LABEL_Q_ARG);
          ascii_arg->label = ascii_label;
          to_return = ascii_arg; // HAS LABEL ABOVE STRING!
  
          quad_arg * ascii = create_quad_arg(LABEL_Q_ARG);
          ascii->label = root->value_string;
  
          gen_quad(GOTO_Q, end_label_arg, NULL, NULL);
          gen_quad(LABEL_Q, ascii_arg, NULL, NULL);
          gen_quad(STRING_Q, ascii, NULL, NULL);
          gen_quad(LABEL_Q, end_label_arg, NULL, NULL);
          break;
        }

      case INT_LITERAL_N:
        to_return = create_quad_arg(INT_LITERAL_Q_ARG);
        to_return->int_literal = root->value_int;
        break;

      case VOID_N:
        to_return = NULL;
        break;

      default:
        {
          ast_node child = root->left_child;

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

quad_arg * CG_assign_op(ast_node root) {
  quad_arg * arg1 = CG(root->left_child->right_sibling);
  quad_arg * arg2 = CG(root->left_child);

  if (arg1->type == TEMP_VAR_Q_ARG) {
    gen_quad(ASSIGN_Q, arg2, arg1, NULL);
  } else {
    temp_var * t3 = new_temp(root);

    quad_arg * arg3 = create_quad_arg(TEMP_VAR_Q_ARG);
    arg3->temp = t3;

    gen_quad(ASSIGN_Q, arg3, arg1, NULL);
    gen_quad(ASSIGN_Q, arg2, arg3, NULL);
  }

  return arg2;
}

quad_arg * CG_math_op(ast_node root, quad_op op) {
  quad_arg * arg1 = CG(root->left_child);

  quad_arg * arg2;
  if (root->left_child->right_sibling == NULL) {

    if (op == NOT_Q || op == NEG_Q) {
      // Special case for not
      gen_quad(op, arg1, NULL, NULL);
    } else {
      // Special case for increment and decrement
      arg2 = create_quad_arg(INT_LITERAL_Q_ARG);
      arg2->int_literal = 1;

      gen_quad(op, arg1, arg2, NULL);
    }

    return arg1;
  } else {
    arg2 = CG(root->left_child->right_sibling);
  }

  temp_var * t3 = new_temp(root);

  quad_arg * arg3 = create_quad_arg(TEMP_VAR_Q_ARG);
  arg3->temp = t3;

  gen_quad(op, arg3, arg1, arg2);

  return arg3;
}

/*
 * returns label of form "L_N[#]_[NODE TYPE]"
 * should be free'd after done using
 */
char * new_label(ast_node root, char * name) {
  if (!root)
    return NULL;

  char * label = (char *)calloc(MAX_LABEL_LENGTH,sizeof(char));
  assert(label);

  sprintf(label,"L_N%d_%s",root->id, name);
  //printf("label: %s\n",label);

  assert(label);
  return label;
}


void print_label(ast_node root) {
  if (!root)
    return;

  char * label = new_label(root, NODE_NAME(root->node_type));
  printf("%s\n",label);
  free(label);

  for (ast_node child = root->left_child; child != NULL; child = child->right_sibling)
    new_label(child, NODE_NAME(child->node_type));
}

/*
 * Functions to handle quad list and quad structs
 */

// initialize global quad list
quad_arr * init_quad_list() {

  if(!quad_list) {
    quad_list = (quad_arr *)calloc(1,sizeof(quad_arr));
    assert(quad_list);
    quad_list->arr = (quad **)calloc(INIT_QUAD_LIST_SIZE, sizeof(quad *));
    assert(quad_list->arr);

    quad_list->size = INIT_QUAD_LIST_SIZE;
    quad_list->count = 0;
  }

  return quad_list;
}

// create a quad arg struct of type
quad_arg * create_quad_arg(quad_arg_discriminant type) {
  quad_arg * new_arg = (quad_arg *)calloc(1,sizeof(quad_arg));
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

  quad_list->arr[quad_list->count] = (quad *)calloc(1,sizeof(quad));
  quad_list->arr[quad_list->count]->number = quad_list->count;
  quad_list->arr[quad_list->count]->op = operation;
  quad_list->arr[quad_list->count]->args[0] = a1;
  quad_list->arr[quad_list->count]->args[1] = a2;
  quad_list->arr[quad_list->count]->args[2] = a3;

  (quad_list->count)++;
  /* double array size if full */
  if (quad_list->count == quad_list->size) {
    quad_list->size *= 2;
    quad_list->arr = realloc(quad_list->arr,sizeof(quad *) * quad_list->size);
    assert(quad_list->arr);
  }

  return 0;
}

// prints global quad list
void print_quad_list() {
  if (quad_list != NULL) {

    for (int i = 0; i < quad_list->count; i++) {

      if (quad_list->arr[i] != NULL)
        print_quad(quad_list->arr[i]);
      else
        fprintf(stderr,"null quad\n");

    }
  } else {
    fprintf(stderr,"cannot print quad list because list is null\n");
  }
}

// print a quad
void print_quad(quad * q) {

  /* print quad operation */
  printf("%d -- (%s, ",q->number,QUAD_NAME(q->op));

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
        printf("Temp %d",
         q->args[i]->temp->id);
        break;

      case SYMBOL_VAR_Q_ARG:
        printf("Symbol: %s",q->args[i]->label);
        break;

      case SYMBOL_ARR_Q_ARG:
        printf("Symbol: %s [offset: %d]",q->args[i]->label,q->args[i]->int_literal);
        break;

      case SYMBOL_FUNC_Q_ARG:
        printf("Function Symbol: %s",q->args[i]->symnode->name);
        break;

      case LABEL_Q_ARG:
        printf("Label: %s",q->args[i]->label);
        break;

      case RETURN_Q_ARG:
        printf("Returned Value");
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

