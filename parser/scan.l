/*
 * FILE: scan.l
 * AUTHORS: Yondon Fu and Matt McFarland - Delights (CS57 - 16W)
 * 
 * PURPOSE: Flex file for scanner
 *
 * ATTRIBUTIONS: SWS for inspiration and kwloopup function model (cscan.l)
 * 
 */
 
%option yylineno 
 
%{
#include <string.h>
#include "parser.tab.h"
//#include "toktypes.h"
int kwLookup(char *); 

#define MAXTOKENLENGTH 201

char savedIdText[MAXTOKENLENGTH];
char savedLiteralText[MAXTOKENLENGTH];

%}

white [ \t\n]+
space [ \t]
letter [A-Za-z]
digit [0-9]
id ({letter}|_)({letter}|{digit}|_)*
integer [+|-]?{digit}+
bad_id ({digit})({digit}|{letter}|_)*

%x string MULTICOMMENT SINGLECOMMENT

%%

{white}

{id}          { 
                strncpy(savedIdText, yytext, MAXTOKENLENGTH-1); 
                return kwLookup(yytext); 
}
{integer}     {
                strncpy(savedLiteralText, yytext, MAXTOKENLENGTH-1);
                return INT_T;
}
{bad_id}	    return OTHER_T;

\"            { BEGIN(string); } /* Transition to string state */
<string>\n    {
  BEGIN(INITIAL);
  yytext[--yyleng] = '\0'; /* Remove trailing double-quote */
  return OTHER_T;
}
<string>\\\"  { yymore(); }
<string>\"    {
  BEGIN(INITIAL); /* Return to initial normal state */
  yytext[--yyleng] = '\0'; /* Remove trailing double-quote. Switching states takes care of leading double quote */
  return STRING_T;
}
<string>.     { yymore(); }

"/*"            { BEGIN(MULTICOMMENT); }
<MULTICOMMENT>[^*\n]+ /* Gobble comment */
<MULTICOMMENT>"*" /* Gobble star */
<MULTICOMMENT>\n /* Gobble new line */
<MULTICOMMENT>"*/" {
  BEGIN(INITIAL);
}

"//"          { BEGIN(SINGLECOMMENT); }
<SINGLECOMMENT>[^\n] /* Gobble comment */
<SINGLECOMMENT>\n { BEGIN(INITIAL); }

"<="          return LTE_T;
">="          return GTE_T;
"=="          return EQ_T;
"!="          return NE_T;
"++"          return INCR_T;
"--"          return DECR_T;
"&&"          return AND_T;
"||"          return OR_T;
"+"           return '+';
"-"           return '-';
"*"           return '*';
"/"           return '/';
"<"           return '<';
">"           return '>';
";"           return ';';
","           return ',';
"("           return '(';
")"           return ')';
"["           return '[';
"]"           return ']';
"{"           return '{';
"}"           return '}';
"%"           return '%';
"="           return '=';
"!"           return '!';

<<EOF>>       return 0;
.             return OTHER_T;

%%

int kwLookup(char *s) {
  // Define struct that encapsulates keyword string and int token values
  struct kw_token {
    char *keyword;
    int token;
  };
  typedef struct kw_token kw_token;

  static kw_token keywords[] = {
    {"if", IF_T},
    {"else", ELSE_T},
    {"do", DO_T},
    {"while", WHILE_T},
    {"return", RETURN_T},
    {"for", FOR_T},
    {"void", VOID_T},
    {"read", READ_T},
    {"print", PRINT_T},
    {"int", TYPEINT_T},
    {NULL, ID_T}
  };

  kw_token *kw;

  // Set end of keywords array (ID_T) to the string
  keywords[sizeof(keywords) / sizeof(keywords[0]) - 1].keyword = s;
  
  // If the end of the array is reached without matching a keyword, string defaults to an ID_T token
  for (kw = keywords; strcmp(kw->keyword, s) != 0; kw++);

  return kw->token;

}


