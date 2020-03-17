#include<ctype.h>
typedef enum {
	BLANK_LINE,
	A_INST,
	C_INST
}LINE_TYPE;

int parse_line(const char* line, char* parsed_line, LINE_TYPE* type);
