/* 
 * ast.h
 *
 * File defining an enum and a struct for node types in an abstract
 * syntax tree.
 *
 * Written by THC for CS 57; massaged by SWS.  
 *
 * You should modify this file as appropriate.
 *
 */

#ifndef AST_H_
#define AST_H_

/* You should fill in the various AST node types.  The following are given
   to you to start with.  You may add or remove node types as you
   wish. */
typedef enum { ROOT_N,
         DECLARATION_LIST_N,

         VAR_N,
         ARG_LIST_N,
         ARGS_N, 
         EXPRESSION_N,
         INT_LITERAL_N,
         ID_N,
         CALL_N,

	       SEQ_N,

	       OP_ASSIGN_N, OP_PLUS_N, OP_MINUS_N, OP_NEG_N, OP_TIMES_N, OP_DIVIDE_N,
	       OP_MOD_N, OP_LT_N, OP_LTE_N, OP_GT_N, OP_GTE_N, 
         OP_EQ_N, OP_NE_N, OP_AND_N, OP_OR_N, OP_NOT_N,
         OP_INC_N, OP_DECR_N,



	       IF_STMT_N, IF_ELSE_STMT_N} ast_node_type;



typedef struct {
        int val;
        char *name;
} val_name_pair;


/* Define a table of nd associated strings.  You
   should modify this table as appropriate.  The order of entries
   should match the order of enumerated values in ast_node_type. */


static val_name_pair token_table[] = {
  { ROOT_N, "ROOT" },
  { DECLARATION_LIST_N, "DECLARATION LIST"},

  { VAR_N, "VARIABLE"},
  { ARG_LIST_N, "ARGUMENT LIST"},
  { ARGS_N, "ARGUMENTS"},
  { EXPRESSION_N, "EXPRESSION"},
  { INT_LITERAL_N, "INT_LITERAL"},
  { ID_N, "IDENTIFIER"},
  { CALL_N, "CALL"},

  { SEQ_N, "SEQ" },

  { OP_ASSIGN_N, "=" },
  { OP_PLUS_N, "+" },
  { OP_MINUS_N, "-" },
  { OP_NEG_N, "-" },
  { OP_TIMES_N, "*" },
  { OP_DIVIDE_N, "/" },
  { OP_MOD_N, "%"},
  { OP_LT_N, "<"},
  { OP_LTE_N, "<="},
  { OP_GT_N, ">"},
  { OP_GTE_N, ">="},
  { OP_EQ_N, "==" },
  { OP_NE_N, "!="},
  { OP_AND_N, "&&"},
  { OP_OR_N, "||"},
  { OP_NOT_N, "!"},
  { OP_INC_N, "++"},
  { OP_DECR_N, "--"},

  { IF_STMT_N, "IF_STMT" },
  { IF_ELSE_STMT_N, "IF_ELSE_STMT" },
  { 0, NULL }
};

#define NODE_INDEX(X)    ( (X) - ROOT_N)
#define NODE_NAME(X)     ( token_table[ NODE_INDEX((X)) ].name)


/* Structure for nodes of the abstract syntax tree.  Uses the
   left-child/right-sibling representation, so that each node can have
   a variable number of children.  You should add or remove fields as
   appropriate. */
typedef struct ast_node_struct *ast_node;
struct ast_node_struct {
  ast_node_type node_type;
  ast_node left_child, right_sibling;

  // as most one of these would be nonzero
  char *value_string;		/* for ID */
  int value_int;		/* for INT_LITERAL */
  
};

/* Create a node with a given token type and return a pointer to the
   node. */
ast_node create_ast_node(ast_node_type node_type);

/* Print the contents of a subtree of an abstract syntax tree, given
   the root of the subtree and the depth of the subtree root. */
void print_ast(ast_node root, int depth);

#endif
