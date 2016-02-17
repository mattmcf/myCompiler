/* symtab.h
 * Declarations for the symbol table.
 * Written by THC for CS 57.
 * You should extend the structs and functions as appropriate.
 *
 * extended and changed muchly by sean
 *
 * Students: Yondon Fu and Matt McFarland - Delights (CS57 16W)
 * 
 */

#ifndef SYMTAB_H_
#define SYMTAB_H_

#include "ast_stack.h"
#include "types.h"
#include "temp_list.h"
#include "ast.h"

#define NOHASHSLOT -1

/*
 * declaration_specifier is for symbol.sym_type
 */
typedef enum {
  VAR_SYM,
  FUNC_SYM
} declaration_specifier_t;



/*
 * ----- STRUCTS -----
 */

typedef struct var_symbol {
  char * name;
  type_specifier_t type;
  modifier_t modifier;

  variable_specie_t specie;       // used to indicate where this thing should live in reference to the FP
  int offset_of_frame_pointer;    // bytes to subtract from frame pointer to get to bottom most byte of variable
} var_symbol;

typedef struct func_symbol {
  type_specifier_t return_type;
  int arg_count;
  var_symbol * arg_arr;     // array to handle dynamically sized argument parameters

  // what else?
} func_symbol;

typedef union symbol {
    var_symbol v;
    func_symbol f;
} symbol;

typedef struct symnode {
  char *name;	                    /* name in this symnode */
  struct symnode  *next;	       /* next symnode in list */
  struct symhashtable *parent;

  ast_node origin;

  /* Other attributes go here. */
  declaration_specifier_t sym_type;   // enum FUNC_SYM or VAR_SYM - says which union symbol is
  symbol s;       // union of func_symbol and var_symbol

} symnode_t;

/* Hash table for a given scope in a symbol table. */
typedef struct symhashtable {
  char *name;
  int size;      /* size of hash table */
  symnode_t **table;    /* hash table */
  int level;      /* level of scope, 0 is outermost */
  int sibno;                    /* 0 is leftmost */
  struct symhashtable *parent, *child, *rightsib;
  struct ast_stack *scopeStack; /* Stack to keep track of traversed elements in scope */

  /* memory tracking stuff */
  //int local_base_offset;        // number of bytes this local scope lives offset from the frame pointer
  //int local_sp;                 // number of bytes from local_base to top from to first unused spot on local stack
  temp_list * t_list;           // tracks count of local temps

} symhashtable_t;

/* Symbol table for all levels of scope. */
typedef struct {
  symhashtable_t *root, *leaf;
    
} symboltable_t;

/* traversal function */
void traverse_ast_tree(ast_node root, symboltable_t * symtab);

/* handle function declaration nodes */
ast_node handle_func_decl_node(ast_node fdl, symboltable_t * symtab);

/* handle variable declaration line nodes */
void handle_var_decl_line_node(ast_node vdl, symboltable_t * symtab);

/*
 * adds a scope pointer to leaf symbolhashtable to all children
 * - does not change scope though
 */
void add_scope_to_children(ast_node root, symboltable_t * symtab);

/* 
 * makes a variable of type and with modifier 
 * returns variable on stack, should save elsewhere
 */
var_symbol init_variable(char * name, type_specifier_t type, modifier_t mod, variable_specie_t specie);

/* 
 * get_type() : returns enumerated type_specifier (definied in symtab.h)
 * for an ast_node n input
 * 
 * returns NULL_TS is node isn't of recognized type
 */
type_specifier_t get_datatype(ast_node n);

/* Create an empty symnode */
symnode_t * create_symnode(symhashtable_t *hashtable, char *name, ast_node origin);

/* Set the nsame in this node. */
void set_node_name(symnode_t *node, char *name);

/* Set node type */
void set_node_type(symnode_t *node, declaration_specifier_t sym_type);

/* Set fields for variable node */
void set_node_var(symnode_t *node, var_symbol *var);

/* Set fields for func node */
void set_node_func(symnode_t *node, char * name, type_specifier_t type, int arg_count, var_symbol *arg_arr);

/* Does the identifier in this node equal name? */
int name_is_equal(symnode_t *node, char *name);


/* Create an empty symbol table. */
symboltable_t *create_symboltable();


/* Insert an entry into the innermost scope of symbol table.  First
   make sure it's not already in that scope.  Return a pointer to the
   entry. */
symnode_t *insert_into_symboltable(symboltable_t *symtab, char *name, ast_node origin);

/* Lookup an entry in a symbol table.  If found return a pointer to it.
   Otherwise, return NULL */
symnode_t *lookup_in_symboltable(symboltable_t *symtab, char *name);

/* Lookup an entry in a symbol hash table. Returns NULL if symnode not found */
symnode_t *lookup_symhashtable(symhashtable_t *hashtable, char *name, int slot);

/* Insert a new entry into a symhashtable, but only if it is not
   already present. */
symnode_t *insert_into_symhashtable(symhashtable_t *hashtable, char *name, ast_node origin);

/* Enter a new scope. */
void enter_scope(symboltable_t *symtab, ast_node node, char *name);

/* Leave a scope. */
void leave_scope(symboltable_t *symtab);

/* print the table in a nice way */
void print_symtab(symboltable_t *symtab);

void print_symhash(symhashtable_t *symtab);

#endif
