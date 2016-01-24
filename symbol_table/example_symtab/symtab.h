/* symtab.h
 * Declarations for the symbol table.
 * Written by THC for CS 57.
 * You should extend the structs and functions as appropriate.
 *
 * extended and changed muchly by sean
 */

#ifndef SYMTAB_H_
#define SYMTAB_H_





typedef struct symnode {
  char *name;	                    /* name in this symnode */
  struct symnode  *next;	    /* next symnode in list */
  struct symhashtable *parent;
  /* Other attributes go here. */
} symnode_t;

/* Set the name in this node. */
void set_node_name(symnode_t *node, char *name);

/* Does the identifier in this node equal name? */
int name_is_equal(symnode_t *node, char *name);


/* Hash table for a given scope in a symbol table. */

typedef struct symhashtable {
  char *name;
  symtab_type  type;
  int size;			/* size of hash table */
  symnode_t **table;		/* hash table */
  int level;			/* level of scope, 0 is outermost */
  int sibno;                    /* 0 is leftmost */
  struct symhashtable *parent, *child, *rightsib;
  
} symhashtable_t;



/* Symbol table for all levels of scope. */
typedef struct {
  symhashtable_t *root, *leaf;
    
} symboltable_t;

/* Create an empty symbol table. */
symboltable_t *create_symboltable();


/* Insert an entry into the innermost scope of symbol table.  First
   make sure it's not already in that scope.  Return a pointer to the
   entry. */
symnode_t *insert_into_symboltable(symboltable_t *symtab, char *name);

/* Lookup an entry in a symbol table.  If found return a pointer to it.
   Otherwise, return NULL */
symnode_t *lookup_in_symboltable(symboltable_t *symtab, char *name);

/* Enter a new scope. */
void enter_scope(symboltable_t *symtab, int type, ast_node node);

/* Leave a scope. */
void leave_scope(symboltable_t *symtab);

/* print the table in a nice way */
void print_symtab(symboltable_t *symtab);

#endif
