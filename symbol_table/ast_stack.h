/*
 * FILE: stack.h
 * DESC: stack for holding ast_node object references
 *
 * AUTHORS: Yondon Fu and Matt McFarland - Delights (CS57 - 16W)
 */

#ifndef AST_STACK_H
#define AST_STACK_H

#include "ast.h"

typedef struct ast_stack {
	int size;			// max elements
	int top; 			// points to lowest empty element

	ast_node * stack;
} ast_stack;

/*
 * InitASTstack() : initilizes an ast_stack of 'size' (>= 1)
 *
 * returns pointer to stack if successful,
 * returns NULL on failure
 */
ast_stack * InitASTstack(int size);

/*
 * Pop() : pops top ast_node pointer off of stack
 * 
 * returns NULL if stack is empty
 */
ast_node ASTPop(ast_stack * stk);

/*
 * Push() : pushes ast_node pointer on top of stack
 * 			(stack automatically resizes)
 *
 * Note: can't push NULL pointers.
 * 
 * returns pushed element if success
 * else returns NULL
 */
ast_node ASTPush(ast_node add, ast_stack * stk);

/*
 * DestroyASTstack() : frees an ast_stack
 *
 */
void DestroyASTstack(ast_stack * stk);

#endif	// AST_STACK_H

