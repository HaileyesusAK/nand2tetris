#ifndef __PARSER_H__
#define __PARSER_H__

#include <ctype.h>
#include "symbol_table.h"

typedef enum {
	BLANK_LINE,		//a blank line or a comment line
	A_INST,
	C_INST,
	LABEL
}LINE_TYPE;

/*
	Parse and categorize a hack assembly instruction. Whitespaces 
	in the instruction and a trailing comment will be stripped away.

	Parameters:
		line[IN]: a valid hack assembly instruction
		parsed_line[OUT]: the parsed instruction
		type[OUT]:  type of the struction
	
	Return values:
		0: success
	   -1: One of the input pointers is NULL.
	
	Note:
		Makesure parsed_line has enough space to contain the parsing result.
			
*/
int parse_line(const char* line, char* parsed_line, LINE_TYPE* type);


/*
	Collects the symbols from the given file and builds a symbol 
	table following Hack assembly contract.

	Parameters:
		asm_file: pointer to a hack assembly file
		symbol_table_ptr: a pointer to the constructed symbol table
	
	Return values:
		0: success
	   -1: One of the input pointers is NULL.
	   -2: error setting up symbol table
	
	Note:
		Once the symbol table is no more required, it must be released
		using sym_table_destroy().
*/
int build_sym_table(FILE *asm_file, void** symbol_table_ptr);

#endif
