/* symtab.c
 * Functions for the symbol table.
 * Written by THC for CS 57.
 *
 * extended and changed by sws
 *
 * You should extend the functions as appropriate.
 */


#include "symtab.h"
#include "ast.h"
#include "ast_stack.h"

#define NOHASHSLOT -1
#define INIT_STK_SIZE 10

/*
 * Functions for symnodes.
 */
  
void set_node_name(symnode_t *node, char *name) {
  node->name = name;
}

int name_is_equal(symnode_t *node, char *name) {
  // 1 is true and 0 is false, but is this the convention in C?
  if (strcmp(node->name, name) == 0) {
    return 1;
  } else {
    return 0;
  }
}

/*
 * Functions for symhashtables.
 */

/* Create an empty symhashtable and return a pointer to it.  The
   parameter entries gives the initial size of the table. */
// symhashtable_t *create_symhashtable(int entries, symtab_type type)
symhashtable_t *create_symhashtable(int entries)    // modified function call
{
  symhashtable_t *hashtable = malloc(sizeof(symhashtable_t));
  assert(hashtable);
  // hashtable->type = type;  -- don't use
  hashtable->size = entries;
  hashtable->table = calloc(entries, sizeof(symnode_t));

  assert(hashtable->table);

  // Initialize stack associated with this hashtable and scope
  hashtable->scopeStack = InitASTStack(INIT_STK_SIZE);

  /* anything else ? */
  
  return hashtable;
}

/* Peter Weinberger's hash function, from Aho, Sethi, & Ullman
   p. 436. */
static int hashPJW(char *s, int size) {
  unsigned h = 0, g;
  char *p;

  for (p = s; *p != '\0'; p++) {
      h = (h << 4) + *p;
      if ((g = (h & 0xf0000000)) != 0)
	h ^= (g >> 24) ^ g;
    }

  return h % size;
}

/* Look up an entry in a symhashtable, returning either a pointer to
   the symnode for the entry or NULL.  slot is where to look; if slot
   == NOHASHSLOT, then apply the hash function to figure it out. */
symnode_t *lookup_symhashtable(symhashtable_t *hashtable, char *name,
				   int slot) {
  symnode_t *node;

  assert(hashtable);

  if (slot == NOHASHSLOT)
    slot = hashPJW(name, hashtable->size);

  for (node = hashtable->table[slot];
       node != NULL && !name_is_equal(node, name);
       node = node->next)
    ;

  return node;
}

/* Insert a new entry into a symhashtable, but only if it is not
   already present. */
symnode_t *insert_into_symhashtable(symhashtable_t *hashtable, char *name) {

  assert(hashtable);

  int slot = hashPJW(name, hashtable->size);
  symnode_t *node = lookup_symhashtable(hashtable, name, slot);

  /* error check if node already existed! */

  if (node == NULL) {
    node = create_symnode(name, hashtable);
    node->next = hashtable->table[slot];
    hashtable->table[slot] = node;
  }

  return node;
}




/*
 * Functions for symboltables.
 */

static const int HASHSIZE = 211;

/* Create an empty symbol table. */
symboltable_t  *create_symboltable() {
  symboltable_t *symtab = malloc(sizeof(symboltable_t));
  assert(symtab);

  symhashtable_t *hashtable = create_symhashtable(HASHSIZE);
  hashtable->level = 0;
  hashtable->name = "0";

  symtab->root = hashtable;
  symtab->leaf = hashtable;
  return symtab;
}

/* Insert an entry into the innermost scope of symbol table.  First
   make sure it's not already in that scope.  Return a pointer to the
   entry. */
symnode_t *insert_into_symboltable(symboltable_t *symtab, char *name) {

  assert(symtab);
  assert(symtab->leaf);
  
  symnode_t *node = lookup_symhashtable(symtab->leaf, name, NOHASHSLOT);

  /* error check!! */
  
  if (node == NULL) {
    node = insert_into_symhashtable(symtab->leaf, name);
    return node;
  } else {
    return NULL;
  }  
}

/* Lookup an entry in a symbol table.  If found return a pointer to it.
   Otherwise, return NULL */
symnode_t *lookup_in_symboltable(symboltable_t  *symtab, char *name) {
  symnode_t *node;
  symhashtable_t *hashtable;


  assert(symtab);

  for (node = NULL, hashtable = symtab->leaf;
       (node == NULL) &&  (hashtable != NULL);
       hashtable = hashtable->parent) {
    node = lookup_symhashtable(hashtable, name, NOHASHSLOT);
  }

  return node;
}

/*
  Functions for entering and leaving scope
*/

/* 
 * When entering scope, a new symbol table is created at the innermost scope
*/
// void enter_scope(symboltable_t *symtab, int type, ast_node node)
void enter_scope(symboltable_t *symtab, ast_node node) {
  assert(symtab);

  // Check if current leaf has any children
  if (symtab->leaf->child != NULL) {
    // Child becomes new leaf
    symtab->leaf->child = create_symhashtable(HASHSIZE);
    symtab->leaf->child->level = symtab->leaf->level + 1;
    symtab->leaf->child->sibno = 0;
    symtab->leaf->child->parent = symtab->leaf;
    symtab->leaf = symtab->leaf->child

  } else {
    // Current leaf already has a child, new hash table
    // should become sibling of that child
    symhashtable_t *hashtable;

    // Find last right sib of current leaf
    for (hashtable = symtab->leaf->child;
      hashtable->rightsib != NULL; hashtable = hashtable->rightsib);

    hashtable->rightsib = create_symhashtable(HASHSIZE);
    hashtable->rightsib->level = symtab->leaf->level + 1;
    hashtable->rightsib->sibno = hashtable->sibno + 1;
    hashtable->rightsib->parent = symtab->leaf;
    symtab->leaf = hashtable->rightsib;
  }

  // Label new hash table with node type that begins the new scope
  // i.e. COMPOUND, IF, WHILE, FOR, etc.
  symtab->leaf->name = NODE_NAME(node->node_type);

  // Push current AST node onto scopeStack as the first node
  ASTPush(node, symtab->leaf->scopeStack);
}

/*
 * When leaving scope, parent hash table becomes new leaf representing
 * current innermost scope
 */
void leave_scope(symboltable_t *symtab) {
  assert(symtab);

  symtab->leaf = symtab->leaf->parent;

  // Destroy stack associated with this scope, don't need it 
  // once done with traversal of scope
  DestroyASTSTack(symtab->leaf->scopeStack);
  symtab->leaf->scopeStack = NULL;
}

/*
 * Print contents of symbol table
 */
void print_symtab(symboltable_t *symtab) {
  print_symhash(symtab->root);
}

void print_symhash(symhashtable_t *hashtable) {
  // Print spacing
  for (int i = 0; i < hashtable->level; i++) {
    printf("  ");
  }

  // Print hash table contents
  for (int i = 0; i < hashtable->size; i++) {
    symnode_t *node;

    for (node = hashtable->table[i]; node != NULL; node = node->next) {
      printf("%s", node->name);

      switch (node->declaration_specifier) {
        case VAR_SYM:
          printf("%s", node->symbol->t);
          break;

        case FUNC_SYM:
          printf("%d", node->symbol->return_type);
          printf("%d", node->symbol->arg_count);
          for (int j = 0; j < node->symbol->arg_count; j++) {
            printf("%d", node->symbol->arg_types[j]);
          }

          break;

        default:
          break;
      }
    }
  }

  printf("\n");

  // Recurse on each child
  symhashtable_t *table;
  for (table = hashtable->child; table != NULL; table = table->rightsib) {
    print_symhash(table);
  }

}
