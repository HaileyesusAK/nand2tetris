#ifndef __PARSER_H__
#define __PARSER_H__

#include <ctype.h>

typedef enum {
	BLANK_LINE,
	A_INST,
	C_INST,
	LABEL
}LINE_TYPE;

int parse_line(const char* line, char* parsed_line, LINE_TYPE* type);


#endif
