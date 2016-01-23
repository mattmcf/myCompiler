CC = gcc
CFLAGS = -g
BISONFL = -d -v
FLEXFLAGS = -ll

.PHONY: clean 

.SUFFIXES: .c

.cc.o:
	$(CC) $(CFLAGS) -c $<

parser : lex.yy.o parser.tab.o ast.o parser_main.o
	$(CC) -o $@ $(CFLAGS) lex.yy.o parser.tab.o ast.o parser_main.o $(FLEXFLAGS)

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

tree_main.o : tree_main.c
	#$(CC) -o $@ $<



clean :
	rm -f parser lex.yy.o parser.tab.o parser_main.o ast.o \
	parser.tab.h parser.tab.c lex.yy.c *~ parser.output \
	&& rm -rf results

depend :
	makedepend -- $(CFLAGS) -- parser_main.c

# DO NOT DELETE THIS LINE -- make depend depends on it.
