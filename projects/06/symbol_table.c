#define _GNU_SOURCE
#include<search.h>
#include<stdlib.h>
#include<stdio.h>
#include "symbol_table.h"


void* sym_table_init(size_t n_entry)
{
	int rc;
	struct hsearch_data* htab = calloc(1, sizeof(struct hsearch_data));
	rc = hcreate_r(n_entry, htab);
	if(!rc)
		return NULL;

	return (void*)htab;
}

int sym_table_insert(void* symbol_table, char* symbol, size_t address, size_t* table_address)
{
	if(!symbol_table || !symbol || !table_address)
		return -1;

	struct hsearch_data* htab = (struct hsearch_data *)symbol_table;
	ENTRY item = {symbol, (void*)address};
	ENTRY* entry;

	int rc = hsearch_r(item, ENTER, &entry, htab);
	*table_address = (size_t)entry->data;
	return (rc == 0);
}

int sym_table_lookup(void* symbol_table, char* symbol, size_t* table_address)
{
	if(!symbol_table || !symbol || !table_address)
		return -1;

	int rc = 1;
	struct hsearch_data* htab = (struct hsearch_data *)symbol_table;
	ENTRY item = {symbol, NULL};
	ENTRY* entry;

	hsearch_r(item, FIND, &entry, htab);
	if(entry)
	{
		*table_address = (size_t)entry->data;
		rc = 0;
	}

	return rc;
}
