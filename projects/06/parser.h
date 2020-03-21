#ifndef __PARSER_H__
#define __PARSER_H__

#include <ctype.h>
#include "symbol_table.h"

typedef enum {
	BLANK_LINE,
	A_INST,
	C_INST,
	LABEL
}LINE_TYPE;

int parse_line(const char* line, char* parsed_line, LINE_TYPE* type);

int build_sym_table(FILE *asm_file, void** symbol_table_ptr);

#endif
