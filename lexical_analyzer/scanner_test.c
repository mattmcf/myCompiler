/*
 * scanner_test.c
 * Yondon Fu and Matt McFarland - Delights
 * CS 57 - 16W
 *
 * Tests the yylex() function of our compiler's scanner
 *
 * reads a token from a token file and compares the token to 
 * to a hand coded "answer sheet"
 *
 * USAGE: ./scanner_test [TEST_FILE] [ANSWER_FILE]
 * 
 * to make and access flex's library :use -lfl for Linux; -ll for OSX
 */

#include <stdio.h>		// for reading and writing to files
#include "toktypes.h"  	// enumerated token definitions

extern char *yytext;		//
extern int yyleng; 			// length of yytext
extern int yylex (void); 	// linked with -ll / lfl
extern int yylineno; 		// enable with %option yylineno

#define TOKEN_LEN 100 		// max length of token id string

/* 
 * From cscan.c - SWS
 */ 
// char *token_name(int token) {
//   char tiny_buf[2];

//   if (token > 256) // non-single-char
//     return (tokens[token - FIRST_BIG_TOKEN]);

//   tiny_buf[1] = 0x00; 		// null terminate string
//   tiny_buf[0] = token;		// single character string
//   return strdup(tiny_buf);
// }

int main(int argc, char ** argv) {

	FILE * tok_fp, * ans_fp;
	char buf[TOKEN_LEN] = "";

	if (argc != 3) {
		fprintf(stderr,"error in scanner_test: improper usage. Invoke with ./scanner_test [TEST_FILE] [ANSWER_FILE]\n");
		return 1;
	}

	//tok_fp = fopen(argv[1], "r");
	ans_fp = fopen(argv[2], "r");

	if (!tok_fp || !ans_fp) {
		fprintf(stderr,"error in scanner_test: could not open one of the specified files.\n");
		return 1;
	}

	int rc;
	while ( fscanf(ans_fp, "%s", buf) > 0 ) {
		// yylex() -> returns token types (enumerated int), text stored in global yytext
		// how to yylex to the file we want (not from stdin)?
		// convert token int to string 

		printf("saw %s\n", buf); 						// retrieves next hand coded token

		// compare seen and expected tokens
	}


	//fclose(tok_fp);
	fclose(ans_fp);

	return 0;

}