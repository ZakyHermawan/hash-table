#pragma once

#define HT_PRIME_1 193
#define HT_PRIME_2 389
#define HT_INITIAL_BASE_SIZE 53

typedef struct {
	char* key;
	char* value;
} ht_item;

typedef struct {
	int base_size;
	int size;
	int count;
	ht_item** items;
} ht_hash_table;

static ht_item* ht_new_items(const char* key, int key_len, const char* value, int value_len);
ht_hash_table* ht_new();
static void ht_del_item(ht_item* item);
void ht_del_hash_table(ht_hash_table* ht);
int ht_hash(const char* s, int a, int m);
int ht_get_hash(const char* s, const int num_buckets, const int attempt);
void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* ht, const char* key);
ht_hash_table* ht_new_sized(const int base_size);
void ht_resize(ht_hash_table* ht, const int base_size);
void ht_resize_up(ht_hash_table* ht);
void ht_resize_down(ht_hash_table* ht);
