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
%token ID_T INT_T STRING_T TYPEINT_T IF_T ELSE_T WHILE_T RETURN_T FOR_T VOID_T READ_T PRINT_T ';' ',' '(' ')' '[' ']' '{' '}' DO_T EOF_T COMMENT_T OTHER_T 

/* from flex&bison book: how to resolve if/then/else */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE_T

/* Lowest Precedence */
%right '=' 				/* right? */
%left OR_T
%left AND_T
%left EQ_T NE_T
%left '<' '>' GTE_T LTE_T
%left '+' '-'				/* how to tell minus_t from uranary t? */
%left '*' '/' '%'
%left '!' INCR_T DECR_T UMINUS_T
/* Highest Precedence */

%%

// program : declarationList {
// 	ast_node t = create_ast_node(ROOT_N);
// 	t->left_child = $1;
// 	root = $$ = t; }
// ;

/* 
 * RULE 26
 */
expression : var '=' expression {
	ast_node t = create_ast_node(EXPRESSION_N);
	t->left_child = $1;
	t->left_child->right_sibling = $3;
	$$ = t; }
| r_value {
	ast_node t = create_ast_node(EXPRESSION_N);
	t->left_child = $1;
	$$ = t; }
;

/*
 * RULE 27
 */
var : ID_T {
	ast_node t = create_ast_node(VAR_N);
	t->value_string = strdup(savedIdText);
	$$ = t; }
| ID_T '[' expression ']' {
	ast_node t = create_ast_node(VAR_N);
	t->value_string = strdup(savedIdText);
	t->left_child = $3;
	$$ = t; }
;

/* 
 * RULE 28
 * taken from tree.5.y and modified / expanded for our grammar 
 */
r_value : 
expression '+' expression {
  ast_node t = create_ast_node(OP_PLUS_N);
  t->left_child = $1;
  t->left_child->right_sibling = $3;
  $$ = t; }
| expression '-' expression {
  ast_node t = create_ast_node(OP_MINUS_N);
  t->left_child = $1;
  t->left_child->right_sibling = $3;
  $$ = t; }
| expression '*' expression {
  ast_node t = create_ast_node(OP_TIMES_N);
  t->left_child = $1;
  t->left_child->right_sibling = $3;
  $$ = t; }
| expression '/' expression {
  ast_node t = create_ast_node(OP_DIVIDE_N);
  t->left_child = $1;
  t->left_child->right_sibling = $3;
  $$ = t; }
| expression '%' expression {
  ast_node t = create_ast_node(OP_MOD_N);
  t->left_child = $1;
  t->left_child->right_sibling = $3;
  $$ = t; }
| expression '<' expression {
  ast_node t = create_ast_node(OP_LT_N);
  t->left_child = $1;
  t->left_child->right_sibling = $3;
  $$ = t; }
| expression LTE_T expression {
  ast_node t = create_ast_node(OP_LTE_N);
  t->left_child = $1;
  t->left_child->right_sibling = $3;
  $$ = t; }
| expression '>' expression {
  ast_node t = create_ast_node(OP_GT_N);
  t->left_child = $1;
  t->left_child->right_sibling = $3;
  $$ = t; }
| expression GTE_T expression {
  ast_node t = create_ast_node(OP_GTE_N);
  t->left_child = $1;
  t->left_child->right_sibling = $3;
  $$ = t; }
| expression EQ_T expression {
  ast_node t = create_ast_node(OP_EQ_N);
  t->left_child = $1;
  t->left_child->right_sibling = $3;
  $$ = t; }
| expression NE_T expression {
  ast_node t = create_ast_node(OP_NE_N);
  t->left_child = $1;
  t->left_child->right_sibling = $3;
  $$ = t; } 
| expression AND_T expression {
  ast_node t = create_ast_node(OP_AND_N);
  t->left_child = $1;
  t->left_child->right_sibling = $3;
  $$ = t; } 
| expression OR_T expression {
  ast_node t = create_ast_node(OP_OR_N);
  t->left_child = $1;
  t->left_child->right_sibling = $3;
  $$ = t; } 
| '!' expression {
  ast_node t = create_ast_node(OP_NOT_N);
  t->left_child = $1;
  t->left_child->right_sibling = $2;
  $$ = t; } 
| '-' expression %prec UMINUS_T {
  ast_node t = create_ast_node(OP_NEG_N);
  t->left_child = $2;
  $$ = t; }
| var { $$ = $1; } 									// note, set VAR to EXPR here
| INCR_T var {
  ast_node t = create_ast_node(OP_INC_N);
  t->left_child = $2;
  $$ = t; }
| DECR_T var {
  ast_node t = create_ast_node(OP_DECR_N);
  t->left_child = $2;
  $$ = t; }
| call { $$ = $1; } 								// note, set expr = call here
| '(' expression ')' { $$ = $2; }
| INT_T {
  ast_node t = create_ast_node(INT_LITERAL_N);
  t->value_int = atoi(savedLiteralText); 			// check if works?
  $$ = t; } 
| '(' error ')' { $$ = NULL; }
;

/*
 * RULE 29
 */
call : ID_T '(' args ')' {
	ast_node t = create_ast_node(CALL_N);
	t->value_string = strdup(savedIdText);
	t->left_child = $3;
	$$ = t; }
;


/*
 * RULE 30
 */
args : argList {
	ast_node t = create_ast_node(ARGS_N);
	t->left_child = $1;
	$$ = t; }
| /* empty */ { $$ = NULL; }
;

/* 
 * RULE 31
 */
argList : argList ',' expression { 
	ast_node t = create_ast_node(ARG_LIST_N); 
	t->left_child = $1;
	t->left_child->right_sibling = $3; 
	$$ = t; }
| expression { 
	ast_node t = create_ast_node(ARG_LIST_N); 
	t->left_child = $1;
	$$ = t;  }
;

%%

int yyerror(char *s) {
	parseError = 1;
	fprintf(stderr, "%s at line %d\n", s, yylineno);
	return 0;
}

