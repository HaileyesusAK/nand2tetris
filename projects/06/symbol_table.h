#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#define SYMBOL_LEN	32 

typedef struct {
	char symbol[SYMBOL_LEN];
	size_t address;
}sym_table_entry;

/*
	Creates a symbol table.

	Parameters:
		n_entry:	the number of entries in the table

	Return values:
		A void pointer to the symbol table on success, or NULL on failure.
*/
void* sym_table_init(size_t n_entry);


/*
	Attempts to insert a new symbol into the table. It creates a new entry only
	if the symbol doesn't exist already and there is enough space in the table.

	Parameters:
		symbol_table: the symbol table created by sym_table_init
		item: the (symbol, address) pair to be added in the table
		table_address: the address associated with the symbol

	Return values:
		-1: One of the input pointers is NULL.
		 0: Success
		 1: Failure. The table is full
*/
int sym_table_insert(void* symbol_table, sym_table_entry* item, size_t* table_address);



/*
	Get the address of the symbol.

	Parameters:
		symbol_table: the symbol table created by sym_table_init
		symbol: the symbol to look for
		address: the address associated with the symbol, if found

	Return values:
		-1: One of the input pointers is NULL.
		 0: Symbol found.
		 1: Symbol not found.
*/
int sym_table_lookup(void* symbol_table, char* symbol, size_t* address);



/*
	Destroy the symbol table initialized by sys_table_init function.
	Parameters:
		symbol_table_ptr: A pointer to the symbol table

*/
void sym_table_destroy(void* symbol_table_ptr);

#endif
