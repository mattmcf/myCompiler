/*
 * Yondon Fu and Matt McFarland
 * CS57 - 16W
 * 
 * Flex file 
 */
 
%option yylineno 
 
%{
#include <string.h>
#include "toktypes.h"
int kwLookup(char *);
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

{id}          return kwLookup(yytext);
{integer}     return INT_T;
{bad_id}	  return OTHER_T;

\"            { BEGIN(string); } /* Transition to string state */
<string>\n    {
  BEGIN(INITIAL);
  yytext[--yyleng] = '\0';
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

"+"           return ADD_T;
"-"           return SUB_T;
"*"           return MULT_T;
"/"           return DIV_T;
"<"           return LT_T;
">"           return GT_T;
";"           return SEMIC_T;
","           return COMMA_T;
"("           return OPAREN_T;
")"           return CPAREN_T;
"["           return OSQR_T;
"]"           return CSQR_T;
"{"           return OCRLY_T;
"}"           return CCRLY_T;
"%"           return PCENT_T;
"="			  return ASSIGN_T;
"<="          return LTE_T;
">="          return GTE_T;
"=="          return EQ_T;
"!="          return NE_T;
"++"          return INCR_T;
"--"          return DECR_T;
"&&"          return AND_T;
"||"          return OR_T;
"!"           return NOT_T;

<<EOF>>       return EOF_T;
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


