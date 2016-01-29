/*
 * FILE: parser.y
 * PURPOSE: bison parser -> creates abstract syntax tree or returns errors
 *
 * AUTHOR: SWS -> Thank you!
 * STUDENTS: Yondon Fu and Matt McFarland - Delights (CS 57 - 16W)
 * 
 * Note: 	LC == "Left Child"
 * 			RS == "Right Sibling"
 * 
 */ 

%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h" 			// defines ast node types and functions
#include <assert.h>

#define YYSTYPE ast_node 	// override default node type
#define YYDEBUG 1 			// turn on debugging

#define MAX_ERRORS 6		// will stop parsing after this many syntax errors

/* from .l file */
extern int yylex();
extern char *yytext;
extern int yylineno;
extern char savedIdText[];
extern char savedLiteralText[];

/* from .c main file */
extern ast_node root;
extern int parseError;

/* in this .y file */
int yyerror(char *s);
int errors; 				// global error count

%}

%error-verbose

/* don't change this token identifier order */
%token ID_T INT_T STRING_T TYPEINT_T IF_T ELSE_T DO_T WHILE_T RETURN_T FOR_T VOID_T READ_T PRINT_T '+' '-' '*' '/' '=' '<' '>' LTE_T GTE_T EQ_T NE_T INCR_T DECR_T AND_T OR_T '!' ';' ',' '(' ')' '[' ']' '{' '}' '%' COMMENT_T OTHER_T 

/* from flex&bison book: how to resolve if/then/else */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE_T

/* Lowest Precedence */
%right '=' 			
%left OR_T
%left AND_T
%left EQ_T NE_T
%left '<' '>' GTE_T LTE_T
%left '+' '-'				
%left '*' '/' '%'
%left '!' INCR_T DECR_T UMINUS_T
/* Highest Precedence */

%%

/*
 * RULE 1
 *
 * LC of program is list of DECLARATION_N in RS's
 */
program : declaration_list {
	ast_node t = create_ast_node(ROOT_N);
	t->left_child = $1;
	root = $$ = t; }
;

/*
 * RULE 2
 *
 * A declaration list is composed of a DECLARATION_N with RS of DECLARATION_N nodes in a list
 */
declaration_list : declaration_list declaration {
	ast_node t = $1;
	if (t != NULL) {
		while (t->right_sibling != NULL)
			t = t->right_sibling;
		t->right_sibling = $2;
		$$ = $1;
	} 
	else
		$$ = $2;
	}
| /* empty */ { $$ = NULL; }
;

/*
 * RULE 3
 *
 * Node will be either VAR_DECLARATION_N or FUNC_DECLARATION_N
 */
declaration : var_declaration 	{ $$ = $1; }
| func_declaration 				{ $$ = $1; }
;

/*
 * RULE 4
 * 
 * LC is TYPE_SPEC_N, RSs of LC are VAR_DECL_N nodes
 */
var_declaration : type_specifier var_declaration_list ';' {
	ast_node t = create_ast_node(VAR_DECLARATION_N);
	t->left_child = $1;
	t->left_child->right_sibling = $2; 
	$$ = t; }
;

/*
 * RULE 5 & 9 REPLACEMENT (void and int recognizers)
 *
 * LC will be either VOID_N or TYPEINT_N
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
 * RULE 6
 *
 * var_list_declaration is just a VAR_DECLAR_N with VAR_DECLAR_N on RS
 */
var_declaration_list : var_declaration_list ',' var_decl {
	ast_node t = $1;
	if (t != NULL) {
		while (t->right_sibling != NULL)
			t = t->right_sibling;
		t->right_sibling = $3;
		$$ = $1;
	}
	else 
		$$ = $3;
	}
| var_decl { $$ = $1; }											// must have at least one element in list
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
	int_n->value_int = atoi(savedLiteralText);
 	t->left_child = id_n;
	t->left_child->right_sibling = int_n;
	$$ = t; }
;

/*
 * RULE 8
 *
 * More order weirdness! So each func_declaration MUST have a type and an identifier
 * but it might not have a compound statement. If it doesn't, then the compound statement will be empty
 */
func_declaration : type_specifier ID_T {
	/* embedded action to save function identifer */
	ast_node id_n = create_ast_node(ID_N);
	id_n->value_string = strdup(savedIdText); 
	$2 = id_n;
} '(' formal_params ')' compound_stmt {
	ast_node t = create_ast_node(FUNC_DECLARATION_N);
	ast_node child;

	t->left_child = $1;
	t->left_child->right_sibling = $2;
	t->left_child->right_sibling->right_sibling = $5;
	t->left_child->right_sibling->right_sibling->right_sibling = $7;
	$$ = t; }
;

/*
 * RULE 10
 *
 * Formal parameters: if list, left child is FORMAL_LIST_N
 * 					  if void, left child is VOID_N
 * 					  if unspecified, left child is a VOID_N
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
| /* empty */ {   											// can't just return null because above productions depend on this right sibling
	ast_node t = create_ast_node(FORMAL_PARAMS_N);
	ast_node void_node = create_ast_node(VOID_N);
	t->left_child = void_node; 
	$$ = t;  
	}
;

/*
 * RULE 11
 *
 * left child and all right siblings are each a FORMAL_PARAM_N
 */
formal_list : formal_list ',' formal_param {
	ast_node t = $1;
	if (t != NULL) {
		while (t->right_sibling)
			t = t->right_sibling;
		t->right_sibling = $3;
		$$ = $1;
	}
	else 
		$$ = $3;
	}
| formal_param { $$ = $1; }									// must have 1 parameter in formal_list 
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
 *
 * Sorry, this is weird! So compound statements need to handle some weird cases:
 * 1) There ARE local declarations AND there ARE statements -> L.C. is local_declaration list (right sibs)
 * 		Right sib of L
 * 2) There ARE NO local declaration AND ARE statements -> L.C. is a statement list
 * 3) There ARE local declarations AND AREN'T statements -> L.C. is a local declaration list
 * 4) There AREN'T local declarations AND AREN'T statements -> this node has no children <- bad decision
 *
 * FOR THE FUTURE -> THIS MIGHT CHANGE DEPENDING ON HOW WE DECIDE TO HANDLE THE PARSE TREE
 */
compound_stmt : '{' local_declarations stmt_list '}' {
	ast_node t = create_ast_node(COMPOUND_STMT_N);
	ast_node d = $2;
	ast_node l = $3;

	if (d != NULL && l != NULL) {
		t->left_child = d;
		t->left_child->right_sibling = l;
		// $$ = t;
	} else if (d == NULL && l != NULL) {
		t->left_child = l;
		// $$ = t;
	} else if (d != NULL && l == NULL) {
		t->left_child = d;
		// $$ = t;
	} else { 
		// $$ = t; 
		;
	}

	$$ = t;

	}
;

/*
 * RULE 14
 *
 * LOCAL_DECLARATION_N has a TYPE_SPEC_N as left child and then VAR_DELC_N as right siblings
 * no children means no local variables are declared
 */
local_declarations : local_declarations var_declaration {
	ast_node t = $1;
	if (t != NULL) {
		while (t->right_sibling != NULL)
			t = t->right_sibling;
		t->right_sibling = $2;
		$$ = $1;
	} 
	else
		$$ = $2;

	}
| /* empty */ { $$ = NULL; }
;	

/*
 * RULE 15
 *
 * see rule 26 for types of available statement children
 * each statemnt is left child -> right siblings (x0, x1, x2 ...)
 * no left child if STMT_LIST_N is NULL
 */
stmt_list : stmt_list stmt {
	ast_node t = $1;
	if (t != NULL) {
		while (t->right_sibling != NULL)
			t = t->right_sibling;
		t->right_sibling = $2;
		$$ = $1;
	} else
		$$ = $2;

	}
| /* empty */ { $$ = NULL; }
;

/*
 * RULE 16
 *
 * No "STMT_N" to reduce to. Each production has it's own node type already.
 */
stmt : 
expression_stmt { $$ = $1; }
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
| error ';' { $$ = NULL; }
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
| IF_T '(' error ')' stmt ELSE_T stmt 
	{ $$ = NULL; }
;	

/*
 * RULE 19
 */
while_stmt : WHILE_T '(' expression ')' stmt {
	ast_node t = create_ast_node(WHILE_N);
	t->left_child = $3;
	t->left_child->right_sibling = $5;
	$$ = t; }
| WHILE_T '(' error ')' stmt { $$ = NULL; }
;

/*
 * RULE 20 
 */
do_while_stmt : DO_T stmt WHILE_T '(' expression ')' ';' {
	ast_node t = create_ast_node(DO_WHILE_N);
	assert($2);
	t->left_child = $2;
	t->left_child->right_sibling = $5;
	$$ = t; }
| DO_T stmt WHILE_T '(' error ')' ';' { $$ = NULL; }
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
| FOR_T '(' error ')' error { $$ = NULL;}
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
| RETURN_T error ';' { $$ = NULL; }
;

/*
 * RULE 24
 */
read_stmt : READ_T var ';' {
	ast_node t = create_ast_node(READ_N);
	t->left_child = $2;
	$$ = t; }
| READ_T error ';' { $$ = NULL; }
;

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
| PRINT_T STRING_T {
	/* embedded action to grab string text */
	ast_node str_n = create_ast_node(STRING_N);
	str_n->value_string = strdup(yytext);
	$2 = str_n;
} ';' {
	ast_node t = create_ast_node(PRINT_N);
	t->left_child = $2; 						
	$$ = t; }
| PRINT_T error ';' { $$ = NULL; }
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
  ast_node t = create_ast_node(INT_LITERAL_N);	
  t->value_int = atoi(savedLiteralText); 			
  $$ = t; } 
| '(' error ')' { $$ = NULL; }
;

/*
 * RULE 29
 *
 * left_child is ID_N with ID string saved in value string
 * left_child->right_sibling is NULL if no arguements are given
 */
call : ID_T {
	/* embedded action to save function call ID string */
	ast_node id_n = create_ast_node(ID_N);
	id_n->value_string = strdup(savedIdText);
	$1 = id_n;
} '(' args ')' {
	ast_node t = create_ast_node(CALL_N);
	t->left_child = $1;
	t->left_child->right_sibling = $4;
	$$ = t; }
;

/*
 * RULE 30
 *
 * if ARGS is null, then no arguments
 * if ARGS is not null, then it is a ARGS_LIST_N
 */
args : arg_list { $$ = $1; }
| /* empty */ 	{ $$ = NULL; }
;

/* 
 * RULE 31
 *
 * Each child and sibling under the ARG_LIST_N is an expression.
 */
arg_list : arg_list ',' expression { 
	ast_node t = $1;
	if (t->left_child) {
		t = t->left_child;
		while (t->right_sibling)
			t = t->right_sibling;
		t->right_sibling = $3;
		$$ = $1;
	}

	} 
| expression { 
	ast_node t = create_ast_node(ARG_LIST_N); 
	t->left_child = $1;
	$$ = t;  }
| error { $$ = NULL; }
;

%%

int yyerror(char *s) {
	parseError = 1;
	fprintf(stderr, "%s at line %d\n", s, yylineno);

	if (++errors == MAX_ERRORS) {
		fprintf(stderr,"Too many syntax errors have occurred. Aborting parse attempt.\n");
		exit(1);
	}	

	return 0;
}

