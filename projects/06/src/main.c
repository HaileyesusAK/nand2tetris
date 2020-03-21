#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#include "hack_interpreter.h"
#include "parser.h"
#include "symbol_table.h"

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		fprintf(stderr, "Usage: %s <input file> <ouput file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	FILE *asm_file = fopen(argv[1], "r");
	if(!asm_file)
	{
		fprintf(stderr, "open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	
	FILE *output_file = fopen(argv[2], "w");
	if(!output_file)
	{
		fprintf(stderr, "open: %s\n", strerror(errno));
		fclose(asm_file);
		return EXIT_FAILURE;
	}

	void *symbol_table;
	int rc = build_sym_table(asm_file, &symbol_table);
	if(rc)
	{
		fprintf(stderr, "build_sym_table failed\n");
		fclose(asm_file);
		fclose(output_file);
		return EXIT_FAILURE;
	}

	rewind(asm_file);

	rc = interpret_asm(asm_file, symbol_table, output_file);
	if(rc)
	{
		fprintf(stderr, "interprete_asm failed\n");
		fclose(asm_file);
		fclose(output_file);
		sym_table_destroy(symbol_table);
		return EXIT_FAILURE;
	}
	
	fclose(asm_file);
	fclose(output_file);
	sym_table_destroy(symbol_table);
	return 0;
}
