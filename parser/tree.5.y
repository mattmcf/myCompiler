%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"

#define YYSTYPE ast_node
#define YYDEBUG 1

extern int yylex();
int yyerror(char *s);
extern char *yytext;

extern int lineNumber;
extern ast_node root;
extern int parseError;

extern char savedIdText[];
extern char savedLiteralText[];

%}

%token NUM_T IDENT_T BADTOKEN_T IF_T ELSE_T

/* from flex&bison book: how to resolve if/then/else */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE_T



%right '='
%left '+' '-'
%left '*'
%left UMINUS_T


%%

code : stmtList {
  ast_node t = create_ast_node(ROOT_N);
  t->left_child = $1;
  root = $$ = t; }
;

stmtList : /* empty */ { $$ = NULL; }
| stmtList stmt {
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
;

stmt : exprStmt { $$ = $1; }
| compoundStmt  { $$ = $1; }
| ifStmt        { $$ = $1; }
;

exprStmt : expr ';' { $$ = $1; }
| ';'               { $$ = NULL; }
| error ';'         { $$ = NULL; }
;

compoundStmt : '{' stmtList '}' {
  ast_node t = create_ast_node(SEQ_N);
  t->left_child = $2;
  $$ = t; }
;

ifStmt : IF_T '(' expr ')' stmt   %prec LOWER_THAN_ELSE {
  ast_node t = create_ast_node(IF_STMT_N);
  t->left_child = $3;
  t->left_child->right_sibling = $5;
  $$ = t; }
| IF_T '(' expr ')' stmt ELSE_T stmt {
  ast_node t = create_ast_node(IF_ELSE_STMT_N);
  t->left_child = $3;
  t->left_child->right_sibling = $5;
  t->left_child->right_sibling->right_sibling = $7;
  $$ = t; }
| IF_T '(' error ')' stmt %prec LOWER_THAN_ELSE
{ $$ = NULL; }
| IF_T '(' error ')' stmt ELSE_T stmt { $$ = NULL; }
;


expr :
IDENT_T {
  ast_node t1 = create_ast_node(ID_N);
  t1->value_string = strdup(yytext);
  $1 = t1;
 } '=' expr {
  ast_node t2 = create_ast_node(OP_ASSIGN_N);
  t2->left_child = $1;
  t2->left_child->right_sibling = $4;
  $$ = t2; }
| expr '+' expr {
  ast_node t = create_ast_node(OP_PLUS_N);
  t->left_child = $1;
  t->left_child->right_sibling = $3;
  $$ = t; }
| expr '-' expr {
  ast_node t = create_ast_node(OP_MINUS_N);
  t->left_child = $1;
  t->left_child->right_sibling = $3;
  $$ = t; }
| expr '*' expr {
  ast_node t = create_ast_node(OP_TIMES_N);
  t->left_child = $1;
  t->left_child->right_sibling = $3;
  $$ = t; }
| '-' expr %prec UMINUS_T {
  ast_node t = create_ast_node(OP_NEG_N);
  t->left_child = $2;
  $$ = t; }
| '(' expr ')' { $$ = $2; }
| NUM_T {
  ast_node t = create_ast_node(INT_LITERAL_N);
  t->value_int = atoi(savedLiteralText);
  $$ = t; }
| IDENT_T {
  ast_node t = create_ast_node(ID_N);
  t->value_string = strdup(savedIdText);
  $$ = t; }
| '(' error ')' { $$ = NULL; }
;



%%

int yyerror(char *s) {
  parseError = 1;
  fprintf(stderr, "%s at line %d\n", s, lineNumber);
  return 0;
}
