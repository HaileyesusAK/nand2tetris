#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"

int parse_line(const char* line, char* parsed_line, LINE_TYPE* type)
{
	if(!line || !parsed_line || !type)
		return -1;
	
	*type = BLANK_LINE;
	const char *c = line;
	char *end;
	int len;
	
	//Advance until a non-whitespace character is found
	while(*c && isspace(*c))
		c++;	
	
	if(!*c || *c == '/') 
		return 0;

	*type = (*c == '@') ? A_INST : C_INST;
	sscanf(c, "%[^/]", parsed_line);

	//remove trailing whitespaces from the parsed instruction
	end = parsed_line + strlen(parsed_line) - 1;
	while(isblank(*end))
		*end--;
	*(++end) = '\0';

	return 0;
}
