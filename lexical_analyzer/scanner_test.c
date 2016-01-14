/*
 * FILE: scanner_test.c
 * AUTHORS: Yondon Fu and Matt McFarland - Delights (CS 57 - 16W)
 *
 * PURPOSE: Tests the yylex() function of our compiler's scanner against an "answer sheet"
 *
 * Reads a token from a token file and compares the token to 
 * to a hand coded "answer sheet"
 *
 * USAGE: ./scanner_test [ANSWER_FILE] < [TEST_FILE]
 * 
 * Make note: to make and access flex's library :use -lfl for Linux; -ll for OSX
 */

#include <stdio.h>		// for reading and writing to files
#include <string.h> 	// for strcmp
#include <stdlib.h> 	// for free (to free strings on heap from strdup)
#include "toktypes.h"  	// enumerated token definitions

extern char *yytext;		
extern int yyleng; 			// length of yytext
extern int yylex (void); 	// linked with -ll / lfl
extern int yylineno; 		// enable with %option yylineno

#define TOKEN_LEN 100 		// max length of token id string

/* 
 * From cscan.c
 * Author: SWS
 */ 
char *token_name(int token) {
  char tiny_buf[2];

  if (token > 256) // non-single-char
    return (tok_names[token - FIRST_BIG_TOKEN]);

  tiny_buf[1] = 0x00; 		// null terminate string
  tiny_buf[0] = token;		// single character string
  return strdup(tiny_buf);
}

/*
 * Tests token analysis 
 */
int main(int argc, char ** argv) {

	FILE * tok_fp, * ans_fp;
	char buf[TOKEN_LEN] = "";

	if (argc < 2) {
		fprintf(stderr,"error in scanner_test: improper usage. Invoke with ./scanner_test [ANSWER_FILE] < [TEST_FILE]\n");
		return 1;
	}

	printf("scanner_test: opening answer key file %s\n", argv[1]);
	ans_fp = fopen(argv[1], "r");
	if (!ans_fp) {
		fprintf(stderr,"error in scanner_test: could not open the file %s \n", argv[1]);
		return 1;
	}

	int rc, token, errors = 0;
	char * tok_str = NULL;
	
	/*
	 * 1. Read an answer key token
	 * 2. Read a token from the directed to from stdin
	 * 3. Compare
	 */
	while ( (token = yylex()) != EOF_T ) {
		fscanf(ans_fp, "%s", buf);
		
		tok_str = token_name(token);
		printf("LINE %d -- saw %s. YYlex returns %s. Expecting %s.\n", yylineno, yytext, tok_str, buf);
		 
		if (strcmp(buf, tok_str) != 0) {
			printf("--- Mismatched token! ---\n");
			errors++;
		}
		
		/* 
		 * Would like to free tok_str to plug memory leaks, but can't because token_name
		 * may or may not have called strdup. Breaks if you try to free un-malloced return.
		 */
		// free(tok_str);
	
	}

	if (fscanf(ans_fp, "%s", buf) != EOF) {
		printf("There were still tokens to read in the answer key!\n");
		errors++;
	}

	if (errors)
		printf("\n--- Total Errors: %d\n",errors);
	else
		printf("\n--- No Errors!\n");

	fclose(ans_fp);

	return errors;
}