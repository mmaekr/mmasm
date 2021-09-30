#include "hashtable.h"

// Simple hash function that I created
unsigned long long mmaekr_hash(char* s) {
	unsigned long long ret = 0;
	int i = 0;
	while (s[i]) {
		ret += (s[i] * MAGIC) >> i;
		i++;
	}
	return ret;
}

// Frees a node in a hashtables's bucket's linked list
static void ht_node_destroy(struct hashtable* ht, struct ht_node* node) {
	ht->destructor(node->data);
	node->data = NULL;
	free(node->key);
	node->key = NULL;
	free(node);
	node = NULL;
}

struct hashtable* hashtable_init(void (*destructor)(void*)) {
	struct hashtable* ht = malloc(sizeof(struct hashtable));
	if (!ht)
		return NULL;
	
	ht->hash_fn = mmaekr_hash;
	ht->destructor = destructor;
	ht->no_buckets = INITIAL_BUCKETS;
	ht->no_elements = 0;
	ht->buckets = malloc(ht->no_buckets * sizeof(struct ht_bucket*));
	if (!ht->buckets)
		return NULL;

	// Initialize each bucket
	for (int i = 0; i < ht->no_buckets; i++) {
		ht->buckets[i] = malloc(sizeof(struct ht_bucket));
		ht->buckets[i]->head = NULL;
	}

	return ht;
}

void hashtable_destroy(struct hashtable* ht) {
	// Remove all buckets
	if (ht->buckets) {
		struct ht_node *t1, *t2 = NULL;
		for (int i = 0; i < ht->no_buckets; i++) {
			t1 = ht->buckets[i]->head;
			while (t1) {
				t2 = t1->next;
				ht_node_destroy(ht, t1);
				t1 = t2;
			}
			free(ht->buckets[i]);
			ht->buckets[i] = NULL;
		}
	}
	free(ht->buckets);
	free(ht);
	ht = NULL;
}

void* hashtable_get(struct hashtable* ht, char* key) {
	int bucket_no = ht->hash_fn(key) % ht->no_buckets;

	// Walk through the linked list if there are multiple elements
	struct ht_node* tmp = ht->buckets[bucket_no]->head;
	while (tmp) {
		if (strcmp(tmp->key, key) == 0) {
			return tmp->data;
		}
		tmp = tmp->next;
	}
	
	return NULL;
}

static void __hashtable_insert(struct hashtable* ht, struct ht_bucket** buckets, int no_buckets, char* key, void* val) {
	int bucket_no = ht->hash_fn(key) % no_buckets;

	// If an item already exists in the bucket, add it to the linked list
	// Otherwise, we just set the head of the bucket to the node
	//struct ht_node** tmp = &ht->buckets[bucket_no]->head;
	struct ht_node* tmp = buckets[bucket_no]->head;
	if (tmp) {
		while (tmp->next) {
			if (strcmp(tmp->key, key) == 0) {
				tmp->data = val;
				return;
			}
			tmp = tmp->next;
		}
		// Create new node and add it to the list
		struct ht_node* new_node = malloc(sizeof(struct ht_node));
		new_node->key = strdup(key);
		new_node->data = val;
		new_node->next = NULL;
		tmp->next = new_node;
	} else {
		struct ht_node* new_node = malloc(sizeof(struct ht_node));
		new_node->key = strdup(key);
		new_node->data = val;
		new_node->next = NULL;
		buckets[bucket_no]->head = new_node;
	}
}

static void hashtable_resize(struct hashtable* ht) {
	// Double the current size and add 1
	// Iterate over each element in the old hashtable, rehash it into the new table
	// Free the memory for the old, smaller table
	int new_size = 2*ht->no_buckets + 1;
	struct ht_bucket** tmp = malloc(new_size * sizeof(struct ht_bucket*));

	// Initialize the new bucket array
	for (int i = 0; i < new_size; i++) {
		tmp[i] = malloc(sizeof(struct ht_bucket));
		tmp[i]->head = NULL;
	}

	// Rehash existing data with new size
	if (ht->buckets) {
		struct ht_node *t1, *t2 = NULL;
		for (int i = 0; i < ht->no_buckets; i++) {
			t1 = ht->buckets[i]->head;
			while (t1) {
				t2 = t1->next;
				__hashtable_insert(ht, tmp, new_size, t1->key, t1->data);
				ht_node_destroy(ht, t1);
				t1 = t2;
			}
			free(ht->buckets[i]);
			ht->buckets[i] = NULL;
		}
	}

	ht->no_buckets = new_size;
	free(ht->buckets);
	ht->buckets = tmp;
}

void hashtable_insert(struct hashtable* ht, char* key, void* val) {
	if (ht->no_elements >= ht->no_buckets) {
		hashtable_resize(ht);
	}

	__hashtable_insert(ht, ht->buckets, ht->no_buckets, key, val);
	ht->no_elements++;
}

void* hashtable_remove(struct hashtable* ht, char* key) {
	int bucket_no = ht->hash_fn(key) % ht->no_buckets;
	void* data = NULL;
	struct ht_node** tmp = &ht->buckets[bucket_no]->head;
	struct ht_node* tmp2 = NULL;

	// Removes an element from the linked list
	while (*tmp) {
		if (strcmp((*tmp)->key, key) == 0) {
			tmp2 = *tmp;
			data = tmp2->data;
			*tmp = (*tmp)->next;
			//ht_node_destroy(ht, tmp2);
			ht->no_elements--;
			return data;
		}
		tmp = &(*tmp)->next;
	}
	return NULL;
}

int hashtable_contains_key(struct hashtable* ht, char* key) {
	int bucket_no = ht->hash_fn(key) % ht->no_buckets;

	// Walk through the linked list if there are multiple elements
	struct ht_node* tmp = ht->buckets[bucket_no]->head;
	while (tmp) {
		if (strcmp(tmp->key, key) == 0) {
			return 1;
		}
		tmp = tmp->next;
	}
	
	return 0;
}

int hashtable_is_empty(struct hashtable* ht) {
	return ht->no_elements > 0 ? 0 : 1;
}

int hashtable_num_keys(struct hashtable* ht) {
	return ht->no_elements;
}
