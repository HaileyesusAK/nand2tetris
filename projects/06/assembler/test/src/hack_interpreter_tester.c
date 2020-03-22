#include "hack_interpreter_tester.h"
#include "hack_interpreter.h"
#include "symbol_table.h"
#include "parser.h"
#include "utils.h"

int test_interpret_asm()
{
	int err = 0, rc;
	
	char *asm_file_path = "../data/input/Pong.asm";
	char *output_file_path = "../data/input/Pong.hack";
	char *expected_file_path = "../data/expected/Pong.hach";
	
	FILE* asm_file = fopen(asm_file_path, "w");
	if(!asm_file)
	{
		printf("test_interpreter_asm: cannot open '%s'\n", asm_file_path);
		return 1;
	}

	FILE* output_file = fopen(output_file_path, "w");
	if(!output_file)
	{
		printf("test_interpreter_asm: cannot open '%s'\n", output_file_path);
		fclose(asm_file);
		return 1;
	}
	
	FILE* expected_file = fopen(expected_file_path, "w");
	if(!output_file)
	{
		printf("test_interpreter_asm: cannot open '%s'\n", expected_file_path);
		fclose(asm_file);
		fclose(output_file);
		return 1;
	}

	void *symbol_table;
	rc = build_sym_table(asm_file, &symbol_table);
	if(rc)
	{
		printf("test_interpreter_asm: cannot build symbol table\n");
		fclose(asm_file);
		fclose(output_file);
		fclose(expected_file);
		return 1;
	}

	rewind(asm_file);
	rc = interpret_asm(asm_file, symbol_table, output_file);
	err += assertEqualInt(rc, 0, "interpret_asm failed");
	if(!err)
	{
		rewind(output_file);
		err += assertEqualFile(output_file, expected_file, "");
	}

	fclose(asm_file);
	fclose(output_file);
	fclose(expected_file);
	sym_table_destroy(symbol_table);
	return err;
}
