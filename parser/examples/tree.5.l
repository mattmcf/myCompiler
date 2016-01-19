%option noyywrap

%{
#include <string.h>
#include "tree.tab.h"

int lineNumber = 1;

#define MAXTOKENLENGTH 201

char savedIdText[MAXTOKENLENGTH];
char savedLiteralText[MAXTOKENLENGTH];

%}

%%

[ \t]+      ;
\n          { lineNumber++; }
[A-Za-z_][A-Za-z0-9_]*   { 
  if (strcmp(yytext, "if") == 0)
    return IF_T;
  else if (strcmp(yytext, "else") == 0)
    return ELSE_T;
  else {
    strncpy(savedIdText, yytext, MAXTOKENLENGTH-1);
    return IDENT_T; }
}
[0-9]+      {
  strncpy(savedLiteralText, yytext, MAXTOKENLENGTH-1);
  return NUM_T; }
"="         return '=';
"+"         return '+';
"-"         return '-';
"*"         return '*';
"("         return '(';
")"         return ')';
";"         return ';';
"{"         return '{';
"}"         return '}';
<<EOF>>     return 0;
.           return BADTOKEN_T;
