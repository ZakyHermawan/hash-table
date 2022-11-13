#include <stdio.h>

#include "hash_table.h"

int main() {
	ht_hash_table* ht = ht_new();
	const char k[100] = "key";
	const char v[100] = "val";
	const char k1[100] = "key1";
	const char v1[100] = "val1";
	const char k2[100] = "key2";
	const char v2[100] = "val2";
	
	// insert to hash tabl
	ht_insert(ht, k, v);
	ht_insert(ht, k1, v1);
	ht_insert(ht, k2, v2);

	// read hash table element
	printf("%s\n", ht_search(ht, k));
	printf("%s\n", ht_search(ht, k1));
	printf("%s\n", ht_search(ht, k2));

	// update value
	ht_insert(ht, k, v1);
	printf("%s\n", ht_search(ht, k));

	// delete from hash table
	ht_delete(ht, k1);
	char* c = ht_search(ht, k1);
	if (c == NULL) {
		printf("c has been deleted");
	}

	ht_del_hash_table(ht);
	ht = 0;

	return 0;
}
