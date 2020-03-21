#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "hack_interpreter.h"
#include "symbol_table.h"
#include "parser.h"

#define MAX_LINE_LEN	128
#define MAX_MSG_LEN		128

static int assertNotNull(void *p, char *msg)
{
	if(p == NULL)
	{
		printf("%p == NULL: %s\n", p, msg);
		return 1;
	}
	return 0;
}


static int assertEqualString(char *s1, char *s2, char *msg)
{
	if(strcmp(s1, s2))
	{
		printf("%s != %s: %s\n", s1, s2, msg);
		return 1;
	}
	return 0;
}

static int assertEqualInt(int x, int y, char *msg)
{
	if(x != y)
	{
		printf("%d != %d: %s\n", x, y, msg);
		return 1;
	}
	return 0;
}


int test_parse_line()
{
	char parsed_line[MAX_LINE_LEN];
	char line[MAX_LINE_LEN];
	LINE_TYPE line_type;
	int rc;
	int err = 0;

	char msg[MAX_MSG_LEN];

	//verify bad input are handled properly
	rc = parse_line(NULL, parsed_line, &line_type);
	err += assertEqualInt(rc, -1, "NULL first parameter not caught");

	rc = parse_line("line", NULL, &line_type);
	err += assertEqualInt(rc, -1, "NULL second parameter not caught");

	rc = parse_line("line", parsed_line, NULL);
	err += assertEqualInt(rc, -1, "NULL second parameter not caught");

	//Verify blank lines are discarded
	parsed_line[0] = 'x';
	snprintf(line, MAX_LINE_LEN, "%20s", " ");
	rc = parse_line(line, parsed_line, &line_type);
	snprintf(msg, MAX_MSG_LEN, "Wrong parsed line length. Expected 0, found %zu",
			 strlen(parsed_line));

	err += assertEqualInt(strlen(parsed_line), 0, msg);

	snprintf(msg, MAX_MSG_LEN, "Wrong parsed line type. Expected %d, found %d",
			 BLANK_LINE, line_type);
	err += assertEqualInt(line_type, BLANK_LINE, msg);

	//Verify instruction line is parsed correctly
	char *inst = "@123";
	snprintf(line, MAX_LINE_LEN, "%20s %s  //comment", " ", inst);
	rc = parse_line(line, parsed_line, &line_type);
	snprintf(msg, MAX_MSG_LEN, "Couldn't parse '%s'", inst);
	err += assertEqualInt(rc, 0, msg);

	snprintf(msg, MAX_MSG_LEN, "Wrong parsed instruction. Expected %s, found %s",
			 inst, parsed_line);
	err += assertEqualString(inst, parsed_line, msg);

	snprintf(msg, MAX_MSG_LEN, "Wrong parsed line type. Expected %d, found %d",
			 A_INST, line_type);
	err += assertEqualInt(line_type, A_INST, msg);

	inst = "D = A; JMP";
	char *expected = "D=A;JMP";
	snprintf(line, MAX_LINE_LEN, "%20s %s  //comment", " ", inst);
	rc = parse_line(line, parsed_line, &line_type);

	snprintf(msg, MAX_MSG_LEN, "Couldn't parse '%s'", inst);
	err += assertEqualInt(rc, 0, msg);

	snprintf(msg, MAX_MSG_LEN, "Wrong parsed instruction. Expected '%s', found '%s'",
		     expected, parsed_line);
	err += assertEqualString(expected, parsed_line, msg);

	snprintf(msg, MAX_MSG_LEN, "Wrong parsed line type. Expected %d, found %d",
			 C_INST, line_type);
	err += assertEqualInt(line_type, C_INST, msg);

	return err;
}

int test_build_sym_table()
{
	const char *test_file = "../data/input/symbol_address.asm";
	const char* ground_truth = "../data/expected/symbol_address.csv";

	FILE *asm_file = fopen(test_file, "r");
	if(!asm_file)
	{
		printf("test_build_sym_table: error opening %s: %s\n", test_file, strerror(errno));
		return 1;
	}

	FILE *cmp_file = fopen(ground_truth,  "r");
	if(!cmp_file)
	{
		printf("test_build_sym_table: error opening %s: %s\n", ground_truth, strerror(errno));
		fclose(asm_file);
		return 1;
	}

	void *symbol_table;
	int rc = build_sym_table(asm_file, &symbol_table);
	if(rc)
	{
		printf("test_build_sym_table: build_sym_table failed: %d\n", rc);
		fclose(asm_file);
		fclose(cmp_file);
		return 1;
	}

	char *line = NULL;
	size_t n_char = 0;
	char symbol[SYMBOL_LEN] = {0};
	size_t expected_address = 0;
	size_t stored_address = 0;
	int err = 0;
	char msg[MAX_MSG_LEN];

	while(getline(&line, &n_char, cmp_file) != -1)
	{
		sscanf(line, "%[^,],%zu\n", symbol, &expected_address);
		rc = sym_table_lookup(symbol_table, symbol, &stored_address);

		snprintf(msg, MAX_MSG_LEN, "sym_table_lookup failed on %s. Expected 0, found %d",
				 symbol, rc);
		err += assertEqualInt(rc, 0, msg);

		snprintf(msg, MAX_MSG_LEN, "Wrong address. expected %zu, found %zu",
			 expected_address, stored_address);
		err += assertEqualInt(expected_address, stored_address, msg);

		line = NULL;
		n_char = 0;
	}

	fclose(asm_file);
	fclose(cmp_file);
	sym_table_destroy(symbol_table);

	return err;
}

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
	return 0;
}

int main () {

	int err;

	printf("TESTING parser.parse_line\n");
	err = test_parse_line();
	if(err)
		printf("parse_line FAILED\n");
	else
		printf("parse_line PASSED\n");

	err=0;
	printf("\nTESTING parser.build_sym_table\n");
	err = test_build_sym_table();
	if(err)
		printf("build_sym_table FAILED\n");
	else
		printf("build_sym_table PASSED\n");

	err=0;
	printf("\nTESTING symbol_table module\n");
	err = test_symbol_table();
	if(err)
		printf("symbol_table module FAILED\n");
	else
		printf("symbol_table PASSED\n");


	return 0;
}
