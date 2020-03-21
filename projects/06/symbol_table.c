#define _GNU_SOURCE
#include <search.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "symbol_table.h"

typedef struct {
	size_t n_entries;
	size_t count;
	char **keys;
	struct hsearch_data* htab;
}hash_table;

void* sym_table_init(size_t n_entries)
{
	int rc;

	hash_table* table = calloc(1, sizeof(hash_table));
	if(!table)
		return NULL;

	table->keys = calloc(1, n_entries * sizeof(char*));
	if(!table->keys)
	{
		free(table);
		return NULL;
	}

	table->htab = calloc(1, sizeof(struct hsearch_data));
	if(!table->htab)
	{
		free(table->keys);
		free(table);
		return NULL;
	}

	rc = hcreate_r(n_entries, table->htab);
	if(!rc)
	{
		free(table->keys);
		free(table->htab);
		free(table);
		return NULL;
	}

	table->n_entries = n_entries;
	return (void*)table;
}

int sym_table_insert(void* symbol_table, sym_table_entry* item, size_t* table_address)
{
	hash_table *table = (hash_table *)symbol_table;
	
	if(!table || !item|| !table_address)
		return -1;

	if(table->count == table->n_entries)
		return 1;
	
	ENTRY new_item = {item->symbol, (void*)item->address};
	ENTRY* entry;

	hsearch_r(new_item, FIND, &entry, table->htab);
	if(!entry)
	{
		size_t i = table->count++;
		table->keys[i] = strdup(item->symbol);
		new_item.key = table->keys[i];
		hsearch_r(new_item, ENTER, &entry, table->htab);
	}
	*table_address = (size_t)entry->data;
	return 0;
}

int sym_table_lookup(void* symbol_table, char* symbol, size_t* address)
{
	if(!symbol_table || !symbol || !address)
		return -1;

	int rc = 1;
	struct hsearch_data* htab = ((hash_table*)symbol_table)->htab;
	ENTRY item = {symbol, NULL};
	ENTRY* entry;

	hsearch_r(item, FIND, &entry, htab);
	if(entry)
	{
		*address = (size_t)entry->data;
		rc = 0;
	}

	return rc;
}

void sym_table_destroy(void *symbol_table)
{
	if(symbol_table)
	{
		hash_table *table = (hash_table*)symbol_table;
		if(table->htab)
			hdestroy_r(table->htab);
		
		for(size_t i = 0; i < table->count; ++i)
			free(table->keys[i]);

		free(table->keys);
		free(table);
	}
}
