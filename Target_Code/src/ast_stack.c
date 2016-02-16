/*
 * FILE: stack.c
 * DESC: implementation of ast_stack.h
 *
 * AUTHORS: Yondon Fu and Matt McFarland - Delights (CS57 - 16W)
 */

#include "ast.h"
#include "ast_stack.h"
#include <stdio.h> 			// for error statements
#include <stdlib.h>			// alloc and free stuff

/*
 * initASTstack() : initilizes an ast_stack of 'size'
 *
 * returns pointer to stack if successful,
 * returns NULL on failure
 */
ast_stack * InitASTStack(int size) {

	if (size <= 0) {
		fprintf(stderr,"ast_stack error: cannot make ast_stack of less than size 1\n");
		return NULL;
	}

	ast_stack * new_stk = (ast_stack *)calloc(1, sizeof(ast_stack));
	if (new_stk == NULL) {
		fprintf(stderr,"ast_stack error: memory failure\n");
		return NULL;
	}

	new_stk->size = size;
	new_stk->top = 0;
	new_stk->stack = (ast_node *)calloc(size, sizeof(ast_node));
	if (new_stk->stack == NULL) {
		fprintf(stderr,"ast_stack error: memory failure\n");
		return NULL;
	}

	return new_stk;
}

/*
 * Pop() : pops top ast_node pointer off of stack
 * 
 * returns NULL if stack is empty
 */
ast_node ASTPop(ast_stack * stk) {

	ast_node to_return;

	if (stk == NULL) {
		fprintf(stderr,"ast_stack error: cannot pop from null stack\n");
		return NULL;
	}

	/* check if empty */
	if (stk->top == 0)
		to_return = NULL;

	/* not empty */
	else 
		to_return = stk->stack[--stk->top];	

	return to_return;
}

/*
 * Push() : pushes ast_node pointer on top of stack
 * 			(stack automatically resizes)
 *
 * Note: can't push NULL pointers.
 * 
 * returns pushed element if success
 * else returns NULL
 */
ast_node ASTPush(ast_node add, ast_stack * stk) {

	if (add == NULL || stk == NULL) {
		fprintf(stderr, "ast_stack push error: null node or null stack\n");
		return NULL;
	} 

	/* push 'add' on top of stack */
	stk->stack[stk->top++] = add;

	/* reallocate stack array if full */
	if (stk->top == stk->size) {
		stk->size = stk->size * 2;
		stk->stack = realloc(stk->stack, sizeof(ast_node) * stk->size); 
		if (stk->stack == NULL) {
			fprintf(stderr, "ast_stack error: memory failure\n");
			return NULL;
		}
	}

	/* return pushed element */
	return stk->stack[stk->top - 1];
}

/*
 * Return size of stack
 */
int ASTSize(ast_stack * stk) {

	if (!stk) {
		fprintf(stderr,"ast_stack size error: null stack\n");
		return -1;
	}

	return stk->top;
}

/*
 * DestroyASTstack() : frees an ast_stack
 */
void DestroyASTStack(ast_stack * stk) {

	if (stk) {
		if (stk->stack != NULL)
			free(stk->stack);

		free(stk);
	}

	return;
}
