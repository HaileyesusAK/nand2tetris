#include<stdio.h>
#include<assert.h>
#include "symbol_table.h"

int main()
{
	size_t n_entry = 1024;
	int rc;
	size_t entry_address;

	sym_table_entry item = {"sym", 10};

	void * sym_table = sym_table_init(n_entry);
	assert(sym_table != NULL);

	//Verify bad inputs are handled in sym_table_insert.
	rc = sym_table_insert(NULL, &item, &entry_address);
	assert(rc == -1);

	rc = sym_table_insert(sym_table, NULL, &entry_address);
	assert(rc == -1);

	rc = sym_table_insert(sym_table, &item, NULL);
	assert(rc == -1);

	//Check the address associated with a new symbol
	rc = sym_table_insert(sym_table, &item, &entry_address);
	assert(rc == 0);
	assert(entry_address == item.address);

	//Verify already existing symbol's address is not overwritten
	sym_table_entry item2 = item;
	item2.address += 1;
	rc = sym_table_insert(sym_table, &item2, &entry_address);
	assert(rc == 0);
	assert(entry_address == item.address);

	//Verify bad inputs are handled in sym_table_lookup.
	rc = sym_table_lookup(NULL, item.symbol, &entry_address);
	assert(rc == -1);

	rc = sym_table_lookup(sym_table, NULL, &entry_address);
	assert(rc == -1);

	rc = sym_table_lookup(sym_table, item.symbol, NULL);
	assert(rc == -1);

	//Lookup for existing symbol must return 0 and the associated address
	rc = sym_table_lookup(sym_table, item.symbol, &entry_address);
	assert(rc == 0);
	assert(entry_address == item.address);

	//Lookup for non existing symbol must return 1
	rc = sym_table_lookup(sym_table, "sym2", &entry_address);
	assert(rc == 1);
	sym_table_destroy(sym_table);
	return 0;
}
