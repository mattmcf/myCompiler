
#include <stdlib.h> 		// for NULL
#include <stdio.h>          // for sprintf
#include <assert.h>        

#include "ast.h"
#include "types.h"
#include "toktypes.h"
#include "IR_gen.h"

#define MAX_LABEL_LENGTH 100 	// should be enough?

extern temp_list * temps_list;

void CG(ast_node root) {
  if (root != NULL) {
    switch (root->node_type) {
      // Switch on node types to handle root
      case EXPRESSION_STMT_N:
        break;
      case IF_STMT_N:
        // new temp t1 = new_temp()
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
        break;

      case OP_PLUS_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // t2 = CG(root->left_child->right_sibling)
        // t3 = CG(root->left_child)
        // GenQuad(ADD_Q, t1, t2, t3);
        // return t1
        break;

      case OP_MINUS_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // GenQuad(SUB_Q, t1, t2, t3);
        // return t1
        break;

      case OP_TIMES_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // t2 = CG(root->left_child->right_sibling)
        // t3 = CG(root->left_child)
        // GenQuad(MUL_Q, t1, t2, t3);
        // return t1
        break;

      case OP_DIVIDE_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // t2 = CG(root->left_child->right_sibling)
        // t3 = CG(root->left_child)
        // GenQuad(DIV_Q, t1, t2, t3);
        // return t1
        break;

      case OP_MOD_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // new temp t3 = new_temp()
        // t2 = CG(root->left_child->right_sibling)
        // t3 = CG(root->left_child)
        // GenQuad(MOD_Q, t1, t2, t3);
        // return t1
        break;

      case OP_INC_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // t1 = CG(root->left_child)
        // GenQuad(ADD_Q, t2, t1, 1)
        // return t2
        break;

      case OP_DECR_N:
        // new temp t1 = new_temp()
        // new temp t2 = new_temp()
        // t1 = CG(root->left_child)
        // GenQuad(SUB_Q, t2, t1, 1)
        // return t2
        break;

      case OP_AND_N:
        break;

      case OP_OR_N:
        break;

      case OP_NEG_N:
      case OP_LT_N:
      case OP_GT_N:
      case OP_GTE_N:
      case OP_LTE_N:
      case OP_EQ_N:
      case OP_NE_N:

      case PRINT_N:
      case READ_N:

      default:
        break;

    }

    ast_node child = root->left_child;

    while (child != NULL) {
      CG(child);
      child = child->right_sibling;
    }
  }
}

// void CG(ast_node root) {
//   if (root != NULL) {
//     ast_node child = root->left_child;

//     while (child != NULL) {
//       // Generate code for child
//       switch (child->node_type) {
//         case EXPRESSION_STMT_N:
//           break;
//         case IF_STMT_N:
//           break;
//         case IF_ELSE_STMT_N:
//           break;
//         case FOR_STMT_N:
//           break;
//         case WHILE_N:
//           break;

//         case OP_PLUS_N:
//           // new temp1 for result
//           // new temp2 = CG(arg1)
//           // new temp3 = CG(arg2)
//           // GenQuad(addl, temp1, temp2, temp3);
//           break;

//         case OP_MINUS_N:
//           // new temp1 for result
//           // GenQuad(addl, temp1, arg1, arg2);
//           // return temp1
//           break;

//         case OP_NEG_N:
//         case OP_TIMES_N:
//         case OP_DIVIDE_N:
//         case OP_MOD_N:
//         case OP_LT_N:
//         case OP_GT_N:
//         case OP_GTE_N:
//         case OP_LTE_N:
//         case OP_EQ_N:
//         case OP_NE_N:
//         case OP_AND_N:
//         case OP_OR_N:
//         case OP_INC_N:
//         case OP_DECR_N:

//         case ASSIGN_N:
//           temp_var * t = new_temp
//           // get new temp
//           // new_temp = CG(right hand side)
//           // store temp at destination location (arg1)
//           break;

//         default:
//           break;

//         // Logical operations? Arrays?
//       }

//       // CG(child);
//       child = child->right_sibling;
//     }

//     // Generate code for root
//   }
// }

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


