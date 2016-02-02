CC = gcc
CFLAGS = -g
BISONFL = -d -v
FLEXFLAGS = -ll

.PHONY: clean 

.SUFFIXES: .c

SRC_DIR = src/
SRC_FILES = $(SRC_DIR)ast.c $(SRC_DIR)ast_stack.c $(SRC_DIR)symtab.c $(SRC_DIR)check_sym.c
OBJ_FILES = $(SRC_FILES:.c=.o)

.cc.o:
	$(CC) $(CFLAGS) -c $<

check_symbols : lex.yy.o parser.tab.o check_symbols_main.o $(OBJ_FILES)
	$(CC) -o $@ $(CFLAGS) lex.yy.o parser.tab.o check_symbols_main.o $(OBJ_FILES) $(FLEXFLAGS)

symtab : lex.yy.o parser.tab.o symtab_main.o $(OBJ_FILES)
	$(CC) -o $@ $(CFLAGS) lex.yy.o parser.tab.o symtab_main.o $(OBJ_FILES) $(FLEXFLAGS)

parser_print : lex.yy.o parser.tab.o parser_main.o $(OJB_FILES)
	$(CC) -o $@ $(CFLAGS) lex.yy.o parser.tab.o parser_main.o $(OBJ_FILES) $(FLEXFLAGS)


lex.yy.o : lex.yy.c
	$(CC) -c $(CFLAGS) $<

parser.tab.o : parser.tab.c
	$(CC) -c $(CFLAGS) $<

lex.yy.c : scan.l parser.tab.h
	flex scan.l

parser.tab.h : parser.y
	bison $(BISONFL) $<

parser.tab.c : parser.y
	bison $(BISONFL) $<



clean :
	rm -f symtab parser_print $(SRC_DIR)*.o *.o \
	parser.tab.h parser.tab.c lex.yy.c *~ parser.output \
	&& rm -rf results

depend :
	makedepend -- $(CFLAGS) -- symtab_main.c

# DO NOT DELETE THIS LINE -- make depend depends on it.
