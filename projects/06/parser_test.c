#include <stdio.h>
#include <string.h>
#include <assert.h>
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

int main () {

	test_parse_line();
	return 0;
}
