CC = gcc
CFLAGS = -g
BISONFL = -d -v
FLEXFLAGS = -ll

.PHONY: clean tree1 tree2a tree2b tree2c tree3 tree4 tree4bad tree5

.SUFFIXES: .c

.cc.o:
	$(CC) $(CFLAGS) -c $<

tree : lex.yy.o tree.tab.o ast.o tree_main.o
	$(CC) -o $@ $(CFLAGS) lex.yy.o tree.tab.o ast.o tree_main.o $(FLEXFLAGS)

lex.yy.o : lex.yy.c
	$(CC) -c $(CFLAGS) $<

tree.tab.o : tree.tab.c
	$(CC) -c $(CFLAGS) $<

lex.yy.c : tree.l tree.tab.h
	flex tree.l

tree.tab.h : tree.y
	bison $(BISONFL) $<

tree.tab.c : tree.y
	bison $(BISONFL) $<

tree_main.o : tree_main.c

tree1:
	cp -f tree.1.l tree.l
	cp -f tree.1.y tree.y
	make tree

tree2a:
	cp -f tree.2.l tree.l
	cp -f tree.2a.y tree.y
	make tree

tree2b:
	cp -f tree.2.l tree.l
	cp -f tree.2b.y tree.y
	make tree

tree2c:
	cp -f tree.2.l tree.l
	cp -f tree.2c.y tree.y
	make tree

tree3:
	cp -f tree.2.l tree.l
	cp -f tree.3.y tree.y
	make tree

tree4:
	cp -f tree.2.l tree.l
	cp -f tree.4.y tree.y
	make tree

tree4bad:
	cp -f tree.2.l tree.l
	cp -f tree.4.bad.y tree.y
	make tree

tree5:
	cp -f tree.5.l tree.l
	cp -f tree.5.y tree.y
	make tree

clean :
	rm -f tree lex.yy.o tree.tab.o tree_main.o ast.o \
	tree.tab.h tree.tab.c lex.yy.c *~ tree.output

depend :
	makedepend -- $(CFLAGS) -- tree_main.c

# DO NOT DELETE THIS LINE -- make depend depends on it.
