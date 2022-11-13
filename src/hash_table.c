#include "hash_table.h"
#include "prime.h"

#include <stdlib.h>
#include <string.h>

static ht_item HT_DELETED_ITEM = { NULL, NULL };

/// <summary>
/// create new item for hash table
/// </summary>
/// <param name="key">key of the item</param>
/// <param name="value">value of the item </param>
/// <returns>new created item</returns>
static ht_item* ht_new_items(const char* key, const char* value) {
	ht_item* new_item = (ht_item*)malloc(sizeof(ht_item));
	if (new_item == NULL) {
		return NULL;
	}
	size_t key_len = strlen(key);
	size_t value_len = strlen(value);
	new_item->key = (char*)calloc(key_len+1, sizeof(char));
	new_item->value = (char*)calloc(value_len+1, sizeof(char));

	if (new_item->key == NULL || new_item->value == NULL) {
		return NULL;
	}

	strncpy(new_item->key, key, key_len);
	strncpy(new_item->value, value, value_len);

	return new_item;
}

/// <summary>
/// create new hashtable
/// </summary>
/// <returns>new hash table with base size HT_INITIAL_BASE_SIZE </returns>
ht_hash_table* ht_new() {
	return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

/// <summary>
/// item destructor
/// </summary>
/// <param name="item">item to be deleted</param>
static void ht_del_item(ht_item* item) {
	free(item->key);
	free(item->value);
	free(item);
}

/// <summary>
/// hash table destructor
/// </summary>
/// <param name="ht">hash table to be deleted</param>
void ht_del_hash_table(ht_hash_table* ht) {
	for (int i = 0; i < ht->size; ++i) {
		ht_item* item = ht->items[i];
		if (item != NULL && item != &HT_DELETED_ITEM) {
			ht_del_item(item);
		}
	}
	free(ht->items);
	free(ht);
}

// get hash of string s of hash table with bucket m with prime a (a > 128)
// used by ht_get_hash (dont use this function directly)

/// <summary>
/// get hash of string s for hash table with bucket size m
/// used by ht_get_hash (dont use this function directly)
/// </summary>
/// <param name="str">string to be hashed</param>
/// <param name="prime_num">prime number for hash function (a > 128) </param>
/// <param name="bucket_size">bucket size of hash table</param>
/// <returns>hash value</returns>
static int ht_hash(const char* str, int prime_num, int bucket_size) {
	unsigned long long hashed = 0;
	size_t len_s = strlen(str);
	for (unsigned long long i = 0; i < len_s; ++i) {
		hashed += (unsigned long long)pow(prime_num, len_s - ((i + 1) * (unsigned long long)str[i]));
		hashed = hashed % bucket_size;
	}

	return (int)hashed;
}

/// <summary>
/// perform double hashing to handle hash collission, use this function if you want to hash key
/// </summary>
/// <param name="s"></param>
/// <param name="num_buckets">number of bucket of hash table</param>
/// <param name="attempt">number of hashing attempt (0 if the item never been hashed)</param>
/// <returns></returns>
static int ht_get_hash(const char* str, const int num_buckets, const int attempt) {
	const int hash_a = ht_hash(str, HT_PRIME_1, num_buckets);
	const int hash_b = ht_hash(str, HT_PRIME_2, num_buckets);

	return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

/// <summary>
/// insert key value pair to hash table
/// </summary>
/// <param name="ht">hash table for item to be inserted</param>
/// <param name="key">key of the item</param>
/// <param name="value">value of the item</param>
void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
	const int load = ht->count * 100 / ht->size;
	if (load > 70) {
		ht_resize_up(ht);
	}
	ht_item* item = ht_new_items(key, value);
	int index = ht_get_hash(item->key, ht->size, 0);
	ht_item* curr = ht->items[index];
	int i = 0;
	while (curr != NULL) {
		if (curr != &HT_DELETED_ITEM) {
			// if there is item with the same key exist, just update
			if (strcmp(curr->key, key) == 0) {
				ht_del_item(curr);
				ht->items[index] = item;
				return;
			}
		}
		index = ht_get_hash(item->key, ht->size, ++i);
		curr = ht->items[index];
	}
	ht->items[index] = item;
	ht->count = ht->count + 1;
}

/// <summary>
/// get value from hashtable
/// </summary>
/// <param name="ht">the hash table</param>
/// <param name="key">key of the item</param>
/// <returns>value of item from hash table, NULL if the item didn't exist in hash table</returns>
char* ht_search(ht_hash_table* ht, const char* key) {
	int index = ht_get_hash(key, ht->size, 0);
	ht_item* curr = ht->items[index];
	int i = 0;
	// find empty space in hash table
	while (curr != NULL) {
		if (curr != &HT_DELETED_ITEM) {
			if (strcmp(curr->key, key) == 0) {
				return curr->value;
			}
		}
		index = ht_get_hash(key, ht->size, ++i);
		curr = ht->items[index];
	}
	return NULL;
}


/// <summary>
/// delete key - value pair from hashtable,
/// deleted item from hashtable will be marked as HT_DELTED_ITEM to prevent breaking the hash chain
/// </summary>
/// <param name="ht">hash table for item to be deleted</param>
/// <param name="key">key of the item</param>
void ht_delete(ht_hash_table* ht, const char* key) {
	const int load = ht->count * 100 / ht->size;
	if (load < 10) {
		ht_resize_down(ht);
	}
	int index = ht_get_hash(key, ht->size, 0);
	ht_item* curr = ht->items[index];
	int i = 0;
	while(curr != NULL) {
		if(curr != &HT_DELETED_ITEM) {
			if(strcmp(curr->key, key) == 0) {
				ht_del_item(curr);
				ht->items[index] = &HT_DELETED_ITEM;
				return;
			}
		}
		index = ht_get_hash(key, ht->size, ++i);
		curr = ht->items[index];
	}
	ht->count = ht->count - 1;
}

/// <summary>
/// make new hash table with size of base_size, the size of the hash table is the next prime from base_size
/// </summary>
/// <param name="base_size">base size of the hash table</param>
/// <returns>new hash table with base size of base_size</returns>
static ht_hash_table* ht_new_sized(const int base_size) {
	ht_hash_table* ht = (ht_hash_table*)malloc(sizeof(ht_hash_table));
	if (ht == NULL) {
		return NULL;
	}
	ht->base_size = base_size;
	ht->size = next_prime(base_size);

	ht->count = 0;
	ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
	return ht;
}

/// <summary>
/// resize hash table to base_size
/// </summary>
/// <param name="ht">hash table to be resized</param>
/// <param name="base_size">new base size of the hash table</param>
static void ht_resize(ht_hash_table* ht, const int base_size) {
	// do not resize hash table lower than HT_INITIAL_BASE_SIZE
	if (base_size < HT_INITIAL_BASE_SIZE) {
		return;
	}

	// make new hash table with size base_size
	ht_hash_table* new_ht = ht_new_sized(base_size);
	for (int i = 0; i < ht->size; ++i) {
		ht_item* curr_item = ht->items[i];
		// copy old key-value pair of old hash table to new hash table
		if (curr_item != NULL && curr_item != &HT_DELETED_ITEM) {
			ht_insert(new_ht, curr_item->key, curr_item->value);
		}
	}

	// make new_ht as our ht, delete old hash table
	ht->base_size = new_ht->base_size;
	ht->count = new_ht->count;

	int temp_size = new_ht->size;
	new_ht->size = ht->size;
	ht->size = temp_size;

	ht_item* items = new_ht->items;
	new_ht->items = ht->items;
	ht->items = items;

	ht_del_hash_table(new_ht);
}

/// <summary>
/// increase the size of hash table
/// </summary>
/// <param name="ht">hash table to be resized</param>
static void ht_resize_up(ht_hash_table* ht) {
	const int new_size = ht->size * 2;
	ht_resize(ht, new_size);
}

/// <summary>
/// reduce the size of hash table
/// </summary>
/// <param name="ht">hash table to be resized</param>
static void ht_resize_down(ht_hash_table* ht) {
	const int new_size = ht->size / 2;
	ht_resize(ht, new_size);
}
