/*
 * stack_test.c
 * 
 * tests stack functions
 */

#include <stdio.h>
#include "ast_stack.h"

int main(void) {

	int size1 = 5;

	printf("Testing stack functionality!\n");
	ast_stack * s = InitASTstack(size1);
}