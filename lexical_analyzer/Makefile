# FILE: Makefile
# AUTHORS: Yondon Fu and Matt McFarland - Delights (CS57 - 16W)
# PURPOSE: make and test two scanner implmentations.
# ATTRIBUTIONS: SWS for providing example code in l3demos/flex/Makefile

# Designed for scanner in C, caller in C


CC = gcc
FLEXFLAGS = -ll
# use -lfl for Linux; -ll for OSX

.SUFFIXES: .c

# Scanner program to return matched tokens in STDIN
scan: lex.yy.o cscan.o
	$(CC) -o $@ lex.yy.o cscan.o $(FLEXFLAGS)

# Testing program to match tokens against answer key
scanner_test: lex.yy.o scanner_test.o
	$(CC) -o $@ lex.yy.o scanner_test.o $(FLEXFLAGS) 

.c.o:
	$(CC) -c $< 

lex.yy.c: scan.l
	flex $<

clean:
	rm -f lex.yy.* *.o scan scanner_test *~
	rm -rf ./results
