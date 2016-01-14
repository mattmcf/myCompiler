/*
 * FILE: cscan.c -- reads tokens from stdin
 *
 * AUTHOR: SWS
 *
 * PURPOSE: reads through stdin and returns matched tokens
 *
 * ATTRIBUTIONS: SWS
 */

#include <stdio.h>
#include <string.h>
#include "toktypes.h"

extern char *yytext;
extern int yyleng;
extern int yylex (void);
extern int yylineno;

/*
 * Author: SWS
 */
char *token_name(int token) {
  char tiny_buf[2];

  if (token > 256) // non-single-char
    return (tok_names[token - FIRST_BIG_TOKEN]);

  tiny_buf[1] = 0x00; 			// null terminate single character string
  tiny_buf[0] = token;    		// single character of string
  return strdup(tiny_buf);
}

int main() {
  int token;

  while ((token = yylex()) != EOF_T)
    printf("LINE: %d -- %s: %d \"%s\"\n", yylineno, token_name(token), yyleng, yytext);

  return 0;
}

  /* Some tokens just to check: == 3.14 "quote\"x" -6.02e23 */
