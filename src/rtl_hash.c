#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "rtl_hash.h"

/* 2^31 + 2^29 - 2^25 + 2^22 - 2^19 - 2^16 + 1 */
#define GOLDEN_RATIO_PRIME_32 0x9e370001UL

static uint32_t hash_int(uint32_t val, uint32_t size)
{
	uint32_t hash = val * GOLDEN_RATIO_PRIME_32;

	/* high bits are more random, so use them */
	return hash % size;
}

static uint32_t hash_str(const char *val, uint32_t size)
{
	uint32_t hash = 0;
	uint32_t seed = 131;	/* 31 131 1313 13131 131313 .. */

	while (*val)
		hash = hash * seed + (uint32_t)*val++;

	return hash % size;
}

struct rtl_hash_table *rtl_hash_init(int size, int key_type)
{
	int i;
	struct rtl_hash_table *table;

	if (!(table = calloc(1, sizeof(struct rtl_hash_table))))
		return NULL;

	table->size = size;
	table->key_type = key_type;
	if (!(table->head = calloc(1, sizeof(struct rtl_hash_head) * table->size))) {
		free(table);
		return NULL;
	}

	for (i = 0; i < table->size; i++)
		RTL_INIT_HLIST_HEAD(table->head + i);

	return table;
}

static struct rtl_hash_node *new_hash_node(const void *key, int key_size, const void *value, int value_size)
{
	struct rtl_hash_node *node;

	if (!(node = calloc(1, sizeof(struct rtl_hash_node))))
		return NULL;

	if (!(node->key = calloc(1, key_size))) {
		free(node);
		return NULL;
	}
	memcpy(node->key, key, key_size);

	if (!(node->value = calloc(1, value_size))) {
		free(node->key);
		free(node);
		return NULL;
	}
	memcpy(node->value, value, value_size);

	rtl_hlist_node_init(&node->node);

	return node;
}

int rtl_hash_add(struct rtl_hash_table *table, const void *key, int key_size, const void *value, int value_size)
{
	int offset;
	struct rtl_hash_node *node;

	if (!table)
		return -1;

	if (table->key_type == RTL_HASH_KEY_TYPE_INT) {
		offset = hash_int(*(int *)key, table->size);
	} else if (table->key_type == RTL_HASH_KEY_TYPE_STR) {
		offset = hash_str((char *)key, table->size);
	} else {
		return -1;
	}

	if (!(node = new_hash_node(key, key_size, value, value_size)))
		return -1;

	rtl_hlist_add_head(&node->node, table->head + offset);

	return 0;
}

static int hash_int_find(struct rtl_hash_table *table, int key,
						 struct rtl_hash_node **node, size_t size)
{
	int offset;
	size_t i = 0;
	struct rtl_hash_node *pos;

	offset = hash_int(key, table->size);

	rtl_hash_for_each_entry(pos, table->head + offset) {
		if (*(int *)pos->key == key) {
			if (i < size)
				node[i] = pos;
			i++;
		}
	}

	return i;
}

static int hash_str_find(struct rtl_hash_table *table, const char *key,
						 struct rtl_hash_node **node, size_t size)
{
	int offset;
	size_t i = 0;
	struct rtl_hash_node *pos;

	offset = hash_str(key, table->size);

	rtl_hash_for_each_entry(pos, table->head + offset) {
		if (strcmp((char *)pos->key, key) == 0) {
			if (i < size)
				node[i] = pos;
			i++;
		}
	}

	return i;
}

/*
 * @return: the number of found nodes
 */
int rtl_hash_find(struct rtl_hash_table *table, const void *key,
				  struct rtl_hash_node **node, size_t size)
{
	if (!table)
		return 0;
	if (table->key_type == RTL_HASH_KEY_TYPE_INT) {
		return hash_int_find(table, *(int *)key, node, size);
	} else if (table->key_type == RTL_HASH_KEY_TYPE_STR) {
		return hash_str_find(table, (char *)key, node, size);
	} else {
		return 0;
	}
}

int rtl_hash_del(struct rtl_hash_node *node)
{
	if (!node)
		return -1;

	if (!rtl_hlist_unhashed(&node->node))
		rtl_hlist_del(&node->node);
	else
		return -1;

	free(node->key);
	free(node->value);
	free(node);
	return 0;
}

void rtl_hash_destroy(struct rtl_hash_table *table)
{
	int i;
	struct rtl_hash_node *pos;
	struct rtl_hlist_node *tmp;

	if (!table)
		return;

	for (i = 0; i < table->size; i++) {
		rtl_hash_for_each_entry_safe(pos, tmp, table->head + i) {
			rtl_hash_del(pos);
		}
	}

	free(table->head);
	free(table);
}
