# Makefile for making the lexical analyzer
# Supports a subset of C
# Taken from SWS's Makefile in the Flex example
#
# Yondon Fu and Matt McFarland
# CS57 - 16W - Delights

# Designed for scanner in C, caller in C

#SRCDIR = ./
#FLEX_FILE = scan.l
#EXECUTABLE = scan

CC = gcc
FLEXFLAGS = -ll
# use -lfl for Linux; -ll for OSX

.SUFFIXES: .c

# Scanner and caller both in C
scan: lex.yy.o cscan.o
	$(CC) -o $@ lex.yy.o cscan.o $(FLEXFLAGS)

scanner_test: lex.yy.o scanner_test.o
	$(CC) -o $@ lex.yy.o scanner_test.o $(FLEXFLAGS) 

.c.o:
	$(CC) -c $< 

lex.yy.c: scan.l
	flex $<

clean:
	rm -f lex.yy.* *.o scan scanner_test *~
