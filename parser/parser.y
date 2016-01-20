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

/*
 * RULE 1
 */
program : declaration_list {
	ast_node t = create_ast_node(ROOT_N);
	t->left_child = $1;
	root = $$ = t; }
;

/*
 * RULE 2
 *
 * while > 1 declaration in list, left child is DECLARATION_LIST_N
 * 		with right sibling as a VAR_DECLARATION_N or FUNC_DECLARATION_N
 * For last element of list, left child is either VAR_DECLARATION_N or FUNC_DECLARATION_N
 */
declaration_list : declaration_list declaration {
	ast_node t = create_ast_node(DECLARATION_LIST_N);
	t->left_child = $1;
	t->left_child->right_sibling = $2;
	$$ = t; }
| declaration { $$ = $1; }
;

/*
 * RULE 3
 *
 * Node will be either VAR_DECLARATION_N or FUNC_DECLARATION_N
 * Can omit? nodes will be either VAR_DECLARATION_N or FUNC_DECLARATION_N
 */
declaration : var_declaration 	{ $$ = $1; }
| func_declaration 				{ $$ = $1; }
;

/*
 * RULE 4
 */
var_declaration : type_specifier var_declaration_list ';' {
	ast_node t = create_ast_node(VAR_DECLARATION_N);
	t->left_child = $1;
	t->left_child->right_sibling = $2; 
	$$ = t; }
;

/*
 * RULE 5 & 9 REPLACEMENT
 *
 * Leftchild will be either VOID_N or TYPEINT_N
 */
type_specifier : TYPEINT_T {
	ast_node t = create_ast_node(TYPE_SPEC_N);
	ast_node int_n = create_ast_node(TYPEINT_N);
	t->left_child = int_n;
	$$ = t; }
| VOID_T {	
	ast_node t = create_ast_node(TYPE_SPEC_N);
	ast_node void_n = create_ast_node(VOID_N);
	t->left_child = void_n;
	$$ = t; }
;

/*
 * RULE 5
 * 
 * left child is of node specifier node type -- here only TYPEINT_N
 */
// var_type_specifier : TYPEINT_T {
// 	ast_node t = create_ast_node(VAR_TYPE_SPECIFIER_N);
// 	ast_node typeint_n = create_ast_node(TYPEINT_N);
// 	t->left_child = typeint_n;
// 	$$ = t; }
// ;

/*
 * RULE 6
 *
 * While still declarations in list (>1), left child is VAR_DECLARATION_LIST_N
 * 		and right child is var_declaration
 * If last declaration in list, left child is var_declaration.
 */
var_declaration_list : var_declaration_list ',' var_decl {
	ast_node t = create_ast_node(VAR_DECLARATION_LIST_N);
	t->left_child = $1;
	t->left_child->right_sibling = $3;
	$$ = t; }
| var_decl {
	ast_node t = create_ast_node(VAR_DECLARATION_LIST_N);
	t->left_child = $1;
	$$ = t; }
;

/*
 * RULE 7
 *
 * If only left child -> single variable declaration
 * If right sibling is of type expression -> single variable assignment
 * If right sibling is of INT_N type -> array declaration
 */
var_decl : ID_T {
	ast_node t = create_ast_node(VAR_DECL_N);
	ast_node id_n = create_ast_node(ID_N);
	id_n->value_string = strdup(savedIdText);
	t->left_child = id_n;
	$$ = t; }
| ID_T '=' expression {
	ast_node t = create_ast_node(VAR_DECL_N);
	ast_node id_n = create_ast_node(ID_N);
	id_n->value_string = strdup(savedIdText);
	t->left_child = id_n;
	t->left_child->right_sibling = $3;
	$$ = t; }
| ID_T '[' INT_T ']' {
	ast_node t = create_ast_node(VAR_DECL_N);
	ast_node id_n = create_ast_node(ID_N);
	ast_node int_n = create_ast_node(INT_LITERAL_N);
	id_n->value_string = strdup(savedIdText);
	id_n->value_int = atoi(savedLiteralText);
 	t->left_child = id_n;
	t->left_child->right_sibling = int_n;
	$$ = t; }
;

/*
 * RULE 8
 */
func_declaration : type_specifier ID_T '(' formal_params ')' compound_stmt {
	ast_node t = create_ast_node(FUNC_DECLARATION_N);
	ast_node id_n = create_ast_node(ID_N);
	id_n->value_string = strdup(savedIdText); 				// gets ID?
	t->left_child = $1;
	t->left_child->right_sibling = id_n;
	t->left_child->right_sibling->right_sibling = $4;
	t->left_child->right_sibling->right_sibling->right_sibling = $6;
	$$ = t; }
;

/*
 * RULE 9
 *
 * left child will be either TYPEINT_N or VOID_N depending on specifier
 */
// func_type_specifier : TYPEINT_T {
// 	ast_node t = create_ast_node(FUNC_TYPE_SPEC_N);
// 	ast_node int_n = create_ast_node(TYPEINT_N);
// 	t->left_child = int_n;
// 	$$ = t; }
// | VOID_T {	
// 	ast_node t = create_ast_node(FUNC_TYPE_SPEC_N);
// 	ast_node void_n = create_ast_node(VOID_N);
// 	t->left_child = void_n;
// 	$$ = t; }
// ;

/*
 * RULE 10
 *
 * Formal parameters: if list, left child is FORMAL_LIST_N
 * 					  if void, left child is VOID_N
 * 					  if unspecified, left child is NULL:
 */
formal_params : formal_list {
	ast_node t = create_ast_node(FORMAL_PARAMS_N);
	t->left_child = $1; 
	$$ = t; }
| VOID_T {
	ast_node t = create_ast_node(FORMAL_PARAMS_N);
	ast_node void_node = create_ast_node(VOID_N);
	t->left_child = void_node; 
	$$ = t; }
| /* empty */ {
	ast_node t = create_ast_node(FORMAL_PARAMS_N);
	$$ = t;
}

/*
 * RULE 11
 *
 * When the formal list has exhausted all of the parameters,
 * the left child is a formal param node. 
 */
formal_list : formal_list ',' formal_param {
	ast_node t = create_ast_node(FORMAL_LIST_N);
	t->left_child = $1;
	t->left_child->right_sibling = $3;
	$$ = t; }
| formal_param {
	ast_node t = create_ast_node(FORMAL_LIST_N);
	t->left_child = $1;
	$$ = t; }
;

/* 
 * RULE 12
 *
 * I think this is used for declaration of a parameter for a function
 * Need to differentiate with array and single parameters -> two different N types
 */
formal_param : type_specifier ID_T {
	ast_node t = create_ast_node(FORMAL_PARAM_N);			// save ID string in ID_N at right_sibling
	ast_node id_n = create_ast_node(ID_N);
	id_n->value_string = strdup(savedIdText);
	t->left_child = $1;
	t->left_child->right_sibling = id_n;
	$$ = t; }
| type_specifier ID_T '[' ']' {
	ast_node t = create_ast_node(FORMAL_PARAM_ARR_N);		// save ID string in ID_N at right_sibling
	ast_node id_n = create_ast_node(ID_N);
	id_n->value_string = strdup(savedIdText);
	t->left_child = $1;
	t->left_child->right_sibling = id_n;
	$$ = t; }
;

/*
 * RULE 13
 */
compound_stmt : '{' local_declarations stmt_list '}' {
	ast_node t = create_ast_node(COMPOUND_STMT_N);
	t->left_child = $2;
	t->left_child->right_sibling = $3;
	$$ = t; }
;

/*
 * RULE 14
 */
local_declarations : local_declarations var_declaration {
	ast_node t = create_ast_node(LOCAL_DECLARATIONS_N);
	t->left_child = $1;
	t->left_child->right_sibling = $2;
	$$ = t; }
| /* empty */ {
	ast_node t = create_ast_node(LOCAL_DECLARATIONS_N);
	$$ = t; }
;	

/*
 * RULE 15
 */
stmt_list : stmt_list stmt {
	ast_node t = create_ast_node(STMT_LIST_N);
	t->left_child = $1;
	t->left_child->right_sibling = $2;
	$$ = t; }
| /* empty */ {
	ast_node t = create_ast_node(STMT_LIST_N);
	$$ = t; }
;

/*
 * RULE 16
 *
 * No "STMT_N" to reduce to. Each production has it's own node type already.
 */
stmt : 
expression_stmt { 
	ast_node t = create_ast_node(STMT_N);
	t->left_child = $1;
	$$ = t; }
| compound_stmt { $$ = $1; }
| if_stmt 		{ $$ = $1; }
| while_stmt 	{ $$ = $1; }
| do_while_stmt { $$ = $1; }
| for_stmt 		{ $$ = $1; }
| return_stmt 	{ $$ = $1; }
| read_stmt 	{ $$ = $1; }
| print_stmt 	{ $$ = $1; }
;

/*
 * RULE 17
 *
 * If no expression before semicolon, then no children
 */
expression_stmt : expression ';' {
	ast_node t = create_ast_node(EXPRESSION_STMT_N);
	t->left_child = $1;
	$$ = t; }
| /* empty */ ';' {
	ast_node t = create_ast_node(EXPRESSION_STMT_N);
	$$ = t; }
;

/* 
 * RULE 18
 * 
 * Taken / Adapted from tree.5.y
 * Author: SWS 
 */
if_stmt : IF_T '(' expression ')' stmt   %prec LOWER_THAN_ELSE {
  ast_node t = create_ast_node(IF_STMT_N);
  t->left_child = $3;
  t->left_child->right_sibling = $5;
  $$ = t; }
| IF_T '(' expression ')' stmt ELSE_T stmt {
  ast_node t = create_ast_node(IF_ELSE_STMT_N);
  t->left_child = $3;
  t->left_child->right_sibling = $5;
  t->left_child->right_sibling->right_sibling = $7;
  $$ = t; }
| IF_T '(' error ')' stmt %prec LOWER_THAN_ELSE
	{ $$ = NULL; }
| IF_T '(' error ')' stmt ELSE_T stmt { $$ = NULL; }
;	

/*
 * RULE 19
 */
while_stmt : WHILE_T '(' expression ')' stmt {
	ast_node t = create_ast_node(WHILE_N);
	t->left_child = $3;
	t->left_child->right_sibling = $5;
	$$ = t; }
;

/*
 * RULE 20 
 */
do_while_stmt : DO_T stmt WHILE_T '(' expression ')' ';' {
	ast_node t = create_ast_node(DO_WHILE_N);
	t->left_child = $2;
	t->left_child->right_sibling = $5;
	$$ = t; }
;

/*
 * RULE 21
 */
for_stmt : FOR_T '(' for_header_expr ';' for_header_expr ';' for_header_expr ')' stmt {
	ast_node t = create_ast_node(FOR_STMT_N);
	t->left_child = $3;
	t->left_child->right_sibling = $5; 
	t->left_child->right_sibling->right_sibling = $7;
	t->left_child->right_sibling->right_sibling->right_sibling = $9;
	$$ = t; }
;

/*
 * RULE 22
 *
 * If FOR_HEADER_EXPR matches empty statement, returns a FOR_HEADER_N without children
 */
for_header_expr : expression {
	ast_node t = create_ast_node(FOR_HEADER_N);
	t->left_child = $1;
	$$ = t; }
| /* empty */ {
	ast_node t = create_ast_node(FOR_HEADER_N);
	$$ = t; }
;

/*
 * RULE 23
 * 
 * If returning a value, return expression is held in left child.
 * If no left child, then no return value (void).
 */
return_stmt : RETURN_T ';' {
	ast_node t = create_ast_node(RETURN_N);
	$$ = t; }
| RETURN_T expression ';' {
	ast_node t = create_ast_node(RETURN_N);
	t->left_child = $2;
	$$ = t; }
;

/*
 * RULE 24
 */
read_stmt : READ_T var ';' {
	ast_node t = create_ast_node(READ_N);
	t->left_child = $2;
	$$ = t;
}

/*
 * RULE 25
 *
 * If printing a string, then left child will be STRING_N
 * else if printing expression, left child will be expression
 */
print_stmt : PRINT_T expression ';' {
	ast_node t = create_ast_node(PRINT_N);
	t->left_child = $2;
	$$ = t; } 
| PRINT_T STRING_T ';' {
	ast_node t = create_ast_node(PRINT_N);
	ast_node str_n = create_ast_node(STRING_N);
	str_n->value_string = strdup(yytext);			// correctly getting STRING?
	t->left_child = str_n; 							// save string in STRING_N -- getting string here?
	$$ = t; }
;

/* 
 * RULE 26
 *
 * if left-child has a right sibling, then VAR_N must be assigned the
 * 	evaluated right sibling expression.
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
 *
 * Left child is always an ID_N with ID string saved there
 * Right sibling is null for VAR_N
 * Right sibling is not null for VAR_N if referencing an array
 */
var : ID_T {
	ast_node t = create_ast_node(VAR_N);
	ast_node id_n = create_ast_node(ID_N);
	id_n->value_string = strdup(savedIdText);
	t->left_child = id_n;
	$$ = t; }
| ID_T '[' expression ']' {
	ast_node t = create_ast_node(VAR_N);
	ast_node id_n = create_ast_node(ID_N);
	id_n->value_string = strdup(savedIdText);
	t->left_child = id_n;
	t->left_child->right_sibling = $3;
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
  ast_node t = create_ast_node(INT_LITERAL_N);		// r-value is an INT_LITERAL_N here
  t->value_int = atoi(savedLiteralText); 			// check if works?
  $$ = t; } 
| '(' error ')' { $$ = NULL; }
;

/*
 * RULE 29
 *
 * left_child is ID_N with ID string saved in value string
 * left_child->right_sibling is ARGS_N
 */
call : ID_T '(' args ')' {
	ast_node t = create_ast_node(CALL_N);
	ast_node id_n = create_ast_node(ID_N);
	id_n->value_string = strdup(savedIdText);
	t->left_child = id_n;
	t->left_child->right_sibling = $3;
	$$ = t; }
;

/*
 * RULE 30
 */
args : arg_list {
	ast_node t = create_ast_node(ARGS_N);
	t->left_child = $1;
	$$ = t; }
| /* empty */ { $$ = NULL; }
;

/* 
 * RULE 31
 */
arg_list : arg_list ',' expression { 
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

