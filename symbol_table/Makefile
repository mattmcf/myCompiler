CC = gcc
CFLAGS = -g
BISONFL = -d -v
FLEXFLAGS = -ll

.PHONY: clean 

.SUFFIXES: .c

.cc.o:
	$(CC) $(CFLAGS) -c $<

symtab : lex.yy.o parser.tab.o ast.o symtab_main.o
	$(CC) -o $@ $(CFLAGS) lex.yy.o parser.tab.o ast.o symtab_main.o $(FLEXFLAGS)

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

# Don't need this line -- linked with '.cc.o' target
#symtab_main.o : symtab_main.c
	#m$(CC) -o $@ $<



clean :
	rm -f symtab lex.yy.o parser.tab.o parser_main.o ast.o \
	parser.tab.h parser.tab.c lex.yy.c *~ parser.output \
	&& rm -rf results

depend :
	makedepend -- $(CFLAGS) -- symtab_main.c

# DO NOT DELETE THIS LINE -- make depend depends on it.
