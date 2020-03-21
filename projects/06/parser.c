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
	char *dst;
	
	//Advance until a non-whitespace character is found
	while(*c && isspace(*c))
		c++;	
	
	if(!*c || *c == '/') 
		return 0;

	*type = (*c == '@') ? A_INST : ((*c == '(') ? LABEL : C_INST);
	
	//compact the instruction by removing whitespaces and trailing comment
	dst = parsed_line;
	for(; *c; c++)
	{
	   	if(isspace(*c))
			continue;
		if(*c == '/')
			break;

		*dst++ = *c;
	}
	*dst = '\0';
	
	return 0;
}
