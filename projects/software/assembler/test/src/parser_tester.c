#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "symbol_table.h"
#include "parser.h"
#include "parser_tester.h"
#include "utils.h"

#define MAX_LINE_LEN	128
#define MAX_MSG_LEN		128

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
		sym_table_destroy(symbol_table);
		return 1;
	}

	char line[MAX_LINE_LEN];
	char symbol[SYMBOL_LEN] = {0};
	char msg[MAX_MSG_LEN];
	size_t expected_address = 0;
	size_t stored_address = 0;
	int err = 0;

	while(fgets(line, MAX_LINE_LEN, cmp_file))
	{
		sscanf(line, "%[^,],%zu\n", symbol, &expected_address);
		rc = sym_table_lookup(symbol_table, symbol, &stored_address);

		snprintf(msg, MAX_MSG_LEN, "sym_table_lookup failed on %s. Expected 0, found %d",
				 symbol, rc);
		err += assertEqualInt(rc, 0, msg);

		snprintf(msg, MAX_MSG_LEN, "Wrong address of '%s'. Expected %zu, found %zu",
				 symbol, expected_address, stored_address);
		err += assertEqualInt(stored_address, expected_address, msg);

	}

	fclose(asm_file);
	fclose(cmp_file);
	sym_table_destroy(symbol_table);

	return err;
}
