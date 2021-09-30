#ifndef _SYMTAB_H_
#define _SYMTAB_H_

// Defines a symbol table is just a wrapper around a hashtable

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

typedef struct {
	struct hashtable* ht;
} symtab;

// Initializes a new symbol table
symtab* symtab_create(void);
// Destroys an existing symbol table
void symtab_destroy(symtab*);
// Gets an entry from the symbol table
int symtab_get(symtab*, const char*);
// Inserts an entry into the symbol table
const char* symtab_set(symtab*, const char*, int);
// Determine how many symbols are defined in the symbol table
int symtab_entry_count(symtab*);
// Check if symbol table contains a symbol
int symtab_contains_key(symtab*, const char*);
// Prints the symbol table
void symtab_print(symtab*);

#endif
