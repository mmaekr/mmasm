#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

// Defines the interface for a hashtable
// Hashtable is implemented with linked list for each bucket
// Doubles size every time number of elements exceeds size to stay efficient

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_BUCKETS 51

// This is a random 64-bit prime number
// Pretty sure this helps the hash?
#define MAGIC 16982929816338792601U

struct ht_node {
	char* key;
	void* data;
	struct ht_node* next;
};

struct ht_bucket {
	struct ht_node* head;	
};

struct hashtable {
	int no_buckets;
	int no_elements;
	// Hash function can be customized
	unsigned long long (*hash_fn)(char*);
	
	// Custom destructor function so data can be freed
	void (*destructor)(void*);
	struct ht_bucket** buckets;
};

// Creates a new hashtable
struct hashtable* hashtable_init(void (*)(void*));
// Frees all data associated with a hashtable
void hashtable_destroy(struct hashtable*);
// Gets an element from hashtable using the key
void* hashtable_get(struct hashtable*, char*);
// Insert an element into the hashtable using a key and value
void hashtable_insert(struct hashtable*, char*, void*);
// Remove an element from the hashtable using a key
void* hashtable_remove(struct hashtable*, char*);
// Check if the hashtable contains a specific key
int hashtable_contains_key(struct hashtable*, char*);
// Check if the hashtable is empty
int hashtable_is_empty(struct hashtable*);
// Check the number of elements in the hashtable
int hashtable_num_keys(struct hashtable*);

#endif
