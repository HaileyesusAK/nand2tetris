#include <stdio.h>
#include "symbol_table_tester.h"
#include "hack_interpreter_tester.h"
#include "parser_tester.h"

int main () {

	int err;

	printf("TESTING parser.parse_line\n");
	err = test_parse_line();
	if(err)
		printf("parse_line FAILED\n");
	else
		printf("parse_line PASSED\n");

	err=0;
	printf("\nTESTING parser.build_sym_table\n");
	err = test_build_sym_table();
	if(err)
		printf("build_sym_table FAILED\n");
	else
		printf("build_sym_table PASSED\n");

	err=0;
	printf("\nTESTING symbol_table module\n");
	err = test_symbol_table();
	if(err)
		printf("symbol_table module FAILED\n");
	else
		printf("symbol_table PASSED\n");
	
	err=0;
	printf("\nTESTING interpreter module\n");
	err = test_interpret_asm();
	if(err)
		printf("interpreter module FAILED\n");
	else
		printf("interpreter PASSED\n");


	return 0;
}
