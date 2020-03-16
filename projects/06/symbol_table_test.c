#include<stdio.h>
#include<assert.h>
#include "symbol_table.h"

int main()
{
	size_t n_entry = 1024;
	int rc;
	char *symbol = "sym";
	size_t address = 10;
	size_t entry_address;

	void * sym_table = sym_table_init(n_entry);
	assert(sym_table != NULL);

	//Verify bad inputs are handled in sym_table_insert.
	rc = sym_table_insert(NULL, symbol, address, &entry_address);
	assert(rc == -1);

	rc = sym_table_insert(sym_table, NULL, address, &entry_address);
	assert(rc == -1);

	rc = sym_table_insert(sym_table, symbol, address, NULL);
	assert(rc == -1);

	//Check the address associated with a new symbol
	rc = sym_table_insert(sym_table, symbol, address, &entry_address);
	assert(rc == 0);
	assert(entry_address == address);

	//Verify already existing symbol's address is not overwritten
	rc = sym_table_insert(sym_table, symbol, address + 1, &entry_address);
	assert(rc == 0);
	assert(entry_address == address);

	//Verify bad inputs are handled in sym_table_lookup.
	rc = sym_table_lookup(NULL, symbol, &entry_address);
	assert(rc == -1);

	rc = sym_table_lookup(sym_table, NULL, &entry_address);
	assert(rc == -1);

	rc = sym_table_lookup(sym_table, symbol, NULL);
	assert(rc == -1);

	rc = sym_table_lookup(sym_table, symbol, &entry_address);
	assert(rc == 0);

	//Lookup for existing symbol must return 0 and the associated address
	rc = sym_table_lookup(sym_table, symbol, &entry_address);
	assert(rc == 0);
	assert(entry_address == address);

	//Lookup for non existing symbol must return 1
	rc = sym_table_lookup(sym_table, symbol, &entry_address);
	assert(rc == 0);

	return 0;
}
