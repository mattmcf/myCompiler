
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
        // new temp t1 = new_temp()
        // t1 = CG(root->left_child->right_sibling)
        // GenQuad(ASSIGN_Q, t1)
        // GenQuad(ASSIGN_Q)
        // store temp at destination location (arg1)
        break;

      default:
        break;

      // Logical operations? Arrays?
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


