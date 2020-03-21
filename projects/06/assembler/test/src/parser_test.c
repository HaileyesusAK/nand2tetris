#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "symbol_table.h"
#include "parser.h"

#define MAX_LINE_LEN	128

int test_parse_line()
{
	char parsed_line[MAX_LINE_LEN];
	char line[MAX_LINE_LEN];
	LINE_TYPE line_type;
	int rc;

	//verify bad input are handled properly
	rc = parse_line(NULL, parsed_line, &line_type);
	assert(rc == -1);
	
	rc = parse_line("line", NULL, &line_type);
	assert(rc == -1);

	rc = parse_line("line", parsed_line, NULL);
	assert(rc == -1);
	
	//Verify blank lines are discarded
	parsed_line[0] = '\0';
	snprintf(line, MAX_LINE_LEN, "%20s", " ");
	rc = parse_line(line, parsed_line, &line_type);
	assert(rc == 0);
	assert(strlen(parsed_line) == 0);
	assert(line_type == BLANK_LINE);
	snprintf(line, MAX_LINE_LEN, "%20s\n", " ");
	rc = parse_line(line, parsed_line, &line_type);
	assert(rc == 0);
	assert(strlen(parsed_line) == 0);
	
	//Verify instruction line is parsed correctly
	char *inst = "@123";
	snprintf(line, MAX_LINE_LEN, "%20s %s  //comment", " ", inst);
	rc = parse_line(line, parsed_line, &line_type);
	assert(rc == 0);
	assert(strcmp(parsed_line, inst) == 0);
	assert(line_type == A_INST);
	
	inst = "D=A; JMP";
	snprintf(line, MAX_LINE_LEN, "%20s %s  //comment", " ", inst);
	rc = parse_line(line, parsed_line, &line_type);
	assert(rc == 0);
	assert(strcmp(parsed_line, "D=A;JMP") == 0);
	assert(line_type == C_INST);

	return 0;
}

void test_build_sym_table()
{
	const char *test_file = "test/data/test.asm";
	const char* ground_truth = "test/data/symbol_address.csv";

	sym_table_entry builtin_sym[] = {
		{"R0", 0}, {"R1", 1}, {"R2", 2}, {"R3", 3}, {"R4", 4},
		{"R5", 5}, {"R6", 6}, {"R7", 7}, {"R8", 8}, {"R9", 9},
		{"R10", 10}, {"R11", 11}, {"R12", 12}, {"R13", 13},
		{"R14", 14}, {"R14", 15}, {"SP", 0}, {"LCL", 1}, {"ARG", 2},
		{"THIS", 3}, {"THAT", 4}, {"SCREEN", 16384}, {"KBD", 24576}
	};

	FILE *asm_file = fopen(test_file, "r");
	if(!asm_file)
	{
		fprintf(stderr, "test_build_sym_table: error opening %s: %s\n", test_file, strerror(errno));
		return;
	}
	
	FILE *cmp_file = fopen(ground_truth,  "r");
	if(!cmp_file)
	{
		fprintf(stderr, "test_build_sym_table: error opening %s: %s\n", ground_truth, strerror(errno));
		fclose(asm_file);
		return;
	}

	void *symbol_table;
	int rc = build_sym_table(asm_file, &symbol_table);
	if(rc)
	{
		fprintf(stderr, "test_build_sym_table: build_sym_table failed: %d\n", rc);
		fclose(asm_file);
		fclose(cmp_file);
		return;
	}

	char *line = NULL;
	size_t n_char = 0;
	char symbol[32] = {0};
	size_t expected_address = 0;
	size_t stored_address = 0;
	while(getline(&line, &n_char, cmp_file) != -1)
	{
		sscanf(line, "%[^,],%zu\n", symbol, &expected_address);
		rc = sym_table_lookup(symbol_table, symbol, &stored_address);
		
		assert(rc == 0);
		assert(expected_address == stored_address);
		
		line = NULL;
		n_char = 0;
	}

	fclose(asm_file);
	fclose(cmp_file);
	sym_table_destroy(symbol_table);
}

int main () {

	test_parse_line();
	test_build_sym_table();
	return 0;
}
