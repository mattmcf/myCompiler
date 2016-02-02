/* types.h
 * Header file for enumerated data types for parser / symbol table
 *
 * Students: Yondon Fu and Matt McFarland - Delights (CS57 16W)
 * 
 */

#ifndef _TYPES_H
#define _TYPES_H

#include "stdlib.h"

/*
 * ----- TYPE ENUMERATIONS -----
 */

typedef struct {
        int val;
        char *name;
} val_name_pair;

/* 
 * type_specifier is for symbol.type
 */
typedef enum {
  NULL_TS,
  INT_TS,       // int literals
  VOID_TS,      // void returns
  FUNC_TS
} type_specifier_t;

/* Table for type specifiers */
static val_name_pair type_table[] = {
  {NULL_TS, "NO DATA TYPE"},
  {INT_TS, "INT"},
  {VOID_TS, "VOID"},
  {FUNC_TS, "FUNC"},
  {0, NULL}
};

#define TYPE_INDEX(X) ( (X) - NULL_TS )
#define TYPE_NAME(X) ( type_table[ TYPE_INDEX((X)) ].name)

/*
 * modifies a datatype to include
 * auxillary information like is pointer,
 * array or single data type
 */
typedef enum {
  NULL_DT,	
  SINGLE_DT,
  ARRAY_DT
  // POINTER_DT; // same thing as is array?
} modifier_t;

static val_name_pair modifier_table[] = {
  {NULL_DT, "NO MODIFER"},	
  {SINGLE_DT, "SINGLE"},
  {ARRAY_DT, "ARRAY"},
  {0, NULL}
};

#define MODIFIER_INDEX(X) ( (X) - NULL_DT )
#define MODIFIER_NAME(X) ( modifier_table[ MODIFIER_INDEX((X)) ].name)

#endif 	// _TYPES_H