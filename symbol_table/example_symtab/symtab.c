/* symtab.c
 * Functions for the symbol table.
 * Written by THC for CS 57.
 *
 * extended and changed by sws
 *
 * You should extend the functions as appropriate.
 */


#include "symtab.h"

#define NOHASHSLOT -1

/*
 * Functions for symnodes.
 */



/*
 * Functions for symhashtables.
 */

/* Create an empty symhashtable and return a pointer to it.  The
   parameter entries gives the initial size of the table. */
symhashtable_t *create_symhashtable(int entries, symtab_type type) {
  symhashtable_t *hashtable = malloc(sizeof(symhashtable_t));
  assert(hashtable);
  hashtable->type = type;
  hashtable->size = entries;
  hashtable->table = calloc(entries, sizeof(symnode_t));

  assert(hashtable->table);


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
    node = create_symnode(name,hashtable);
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

  symhashtable_t *hashtable=create_symhashtable(HASHSIZE, GLOBALS_S);
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
