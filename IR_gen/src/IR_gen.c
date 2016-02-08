
#include <stdlib.h> 		// for NULL
#include <stdio.h>          // for sprintf
#include <assert.h>        

#include "ast.h"
#include "types.h"
#include "toktypes.h"

#define MAX_LABEL_LENGTH 100 	// should be enough?

/*
 * returns label of form "L_N[#]_[NODE TYPE]"
 * should be free'd after done using
 */
char * make_label(ast_node root) {
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

  char * label = make_label(root);
  printf("%s\n",label);
  free(label);

  for (ast_node child = root->left_child; child != NULL; child = child->right_sibling)
    print_label(child);
}
	       
