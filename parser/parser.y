/*
 * FILE: parser.y
 * PURPOSE: bison parser -> creates abstract syntax tree or returns errors
 *
 * AUTHOR: SWS -> Thank you!
 * STUDENTS: Yondon Fu and Matt McFarland - Delights (CS 57 - 16W)
 * 
 */ 

%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h" 			// defines ast node types and functions

#define YYSTYPE ast_node 	// override default node type
#define YYDEBUG 1 			// turn on debugging? 

/* from .l file */
extern int yylex();
extern char *yytext;
extern int yylineno;
extern char savedIdText[];
extern char savedLiteralText[];


/* from .c main file */
extern ast_node root;
extern int parseError;
// extern int lineNumber;

/* in this .y file */
int yyerror(char *s);

%}

// all tokens?
// 'do' token?
// %token ID_T, INT_T, STRING_T, TYPEINT_T, IF_T, ELSE_T, WHILE_T, RETURN_T, FOR_T, VOID_T, READ_T, PRINT_T, ADD_T, SUB_T, MULT_T, DIV_T, ASSIGN_T, LT_T, GT_T, LTE_T, GTE_T, EQ_T, NE_T, INCR_T, DECR_T, AND_T, OR_T, NOT_T,SEMIC_T, COMMA_T, OPAREN_T, CPAREN_T, OSQR_T, CSQR_T, OCRLY_T, CCRLY_T, PCENT_T, EOF_T, COMMENT_T, OTHER_T 	/* do ? */
%token ID_T, INT_T, STRING_T, TYPEINT_T, IF_T, ELSE_T, WHILE_T, RETURN_T, FOR_T, VOID_T, READ_T, PRINT_T,SEMIC_T, COMMA_T, OPAREN_T, CPAREN_T, OSQR_T, CSQR_T, OCRLY_T, CCRLY_T, EOF_T, COMMENT_T, OTHER_T 	/* do ? */

/* from flex&bison book: how to resolve if/then/else */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE_T

/* Lowest Precedence */
%right ASSIGN_T 				/* right? */
%left OR_T
%left AND_T
%left EQ_T NE_T
%left LT_T GT_T GTE_T LTE_T
%left ADD_T MINUS_T				/* how to tell minus_t from uranary t? */
%left MULT_T DIV_T PCENT_T
%left NOT_T INCR_T DECR_T MINUS_T
/* Highest Precedence */

%%

program : declarationList {
	ast_node t = create_ast_node(ROOT_N);
	t->left_child = $1;
	root = $$ = t; }
;

// ... better to build bottom up or top down?

%%

int yyerror(char *s) {
	parseError = 1;
	fprintf(stderr, "%s at line %d\n", s, yylineno);
	return 0;
}

