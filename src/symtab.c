#include "symtab.h"

static void symtab_elem_destroy(void* elem) {
	free(elem);
	elem = NULL;
}

symtab* symtab_create(void) {
	symtab* table = malloc(sizeof(symtab));
	table->ht = hashtable_init(symtab_elem_destroy);

	return table;
}

void symtab_destroy(symtab* table) {
	hashtable_destroy(table->ht);
	free(table);
	table = NULL;
}

int symtab_get(symtab* table, const char* label) {
	void* addr = hashtable_get(table->ht, (char*)label);
	if (addr)
		return *(int*)addr;
	return -1;
}

const char* symtab_set(symtab* table, const char* label, int address) {
	int* tmp = malloc(sizeof(int));
	*tmp = address;
	hashtable_insert(table->ht, (char*)label, tmp);
	return label;
}

int symtab_entry_count(symtab* table) {
	return hashtable_num_keys(table->ht);
}

int symtab_contains_key(symtab* table, const char* label) {
	return hashtable_contains_key(table->ht, (char*)label);
}

void symtab_print(symtab* table) {
	//TODO this feels very hashtable specific? maybe doesn't belong in here
	struct hashtable* ht = table->ht;
	struct ht_node* n = NULL;
	for (int i = 0; i < ht->no_buckets; i++) {
		if (ht->buckets[i]->head) {
			n = ht->buckets[i]->head;
			while (n) {
				printf("\t%s : 0x%02x\n", n->key, *(int*)n->data);
				n = n->next;
			}
		}
	}
}
