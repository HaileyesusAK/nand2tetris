#include <stdio.h>
#include "symbol_table.h"
#include "symbol_table_tester.h"
#include "utils.h"

#define MAX_MSG_LEN		128

int test_symbol_table()
{
	size_t n_entry = 1024;
	int rc;
	size_t entry_address;
	char msg[MAX_MSG_LEN];

	sym_table_entry item = {"sym", 10};

	void * sym_table = sym_table_init(n_entry);
	int err = assertNotNull(sym_table, "sym_table_init failed");

	//Verify bad inputs are handled in sym_table_insert.
	rc = sym_table_insert(NULL, &item, &entry_address);
	err += assertEqualInt(rc, -1, "sym_table_insert: NULL first parameter not caught");

	rc = sym_table_insert(sym_table, NULL, &entry_address);
	err += assertEqualInt(rc, -1, "sym_table_insert: NULL second parameter not caught");

	rc = sym_table_insert(sym_table, &item, NULL);
	err += assertEqualInt(rc, -1, "sym_table_insert: NULL third parameter not caught");

	//Check the address associated with the new symbol
	rc = sym_table_insert(sym_table, &item, &entry_address);
	snprintf(msg, MAX_MSG_LEN, "sym_table_insert: failed on %s", item.symbol);
	err += assertEqualInt(rc, 0, msg);

	snprintf(msg, MAX_MSG_LEN,
			 "sym_table_insert: Wrong entry address. Expected %zu, found %zu",
			 item.address, entry_address);
	err += assertEqualInt(item.address, entry_address, msg);

	//Verify already existing symbol's address is not overwritten
	sym_table_entry item2 = item;
	item2.address += 1;
	rc = sym_table_insert(sym_table, &item2, &entry_address);

	snprintf(msg, MAX_MSG_LEN, "sym_table_insert: failed on %s", item2.symbol);
	err += assertEqualInt(rc, 0, msg);

	snprintf(msg, MAX_MSG_LEN,
			 "sym_table_insert: Existing symbol overwritten. Expected %zu, found %zu",
			 item.address, entry_address);
	err += assertEqualInt(item.address, entry_address, msg);

	//Verify bad inputs are handled in sym_table_lookup.
	rc = sym_table_lookup(NULL, item.symbol, &entry_address);
	err += assertEqualInt(rc, -1, "sym_table_lookup: NULL first parameter not caught");

	rc = sym_table_lookup(sym_table, NULL, &entry_address);
	err += assertEqualInt(rc, -1, "sym_table_lookup: NULL second parameter not caught");

	rc = sym_table_lookup(sym_table, item.symbol, NULL);
	err += assertEqualInt(rc, -1, "sym_table_lookup: NULL third parameter not caught");

	//Lookup for existing symbol must return 0 and the associated address
	rc = sym_table_lookup(sym_table, item.symbol, &entry_address);

	snprintf(msg, MAX_MSG_LEN, "sym_table_lookup: failed on %s", item.symbol);
	err += assertEqualInt(rc, 0, msg);

	snprintf(msg, MAX_MSG_LEN,
			 "sym_table_lookup: Wrong symbol address. Expected %zu, found %zu",
			 item.address, entry_address);
	err += assertEqualInt(item.address, entry_address, msg);

	//Lookup for non existing symbol must return 1
	rc = sym_table_lookup(sym_table, "sym2", &entry_address);
	snprintf(msg, MAX_MSG_LEN, "sym_table_lookup: failed on non-existing symbol");
	err += assertEqualInt(rc, 1, msg);
	
	sym_table_destroy(sym_table);
	return err;
}
