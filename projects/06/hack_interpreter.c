#define _GNU_SOURCE
#include<search.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#include "hack_interpreter.h"
#include "symbol_table.h"
#include "parser.h"

#define N_BUILTIN_SYM 23 
#define WSIZE 16
#define JMP_LEN 3
#define DST_LEN 3
#define COMP_LEN 7

#define ERR_BAD_INPUT -1
#define ERR_LOOKUP_NO_SYM -2
#define ERR_LOOKUP_NO_DST -3
#define ERR_LOOKUP_NO_COMP -4
#define ERR_LOOKUP_NO_JMP -5

#define a 3
#define d1 10
#define j1 13

static int interpret_inst(void *symbol_table, void* inst_table, char *asm_inst, char *machine_inst)
{
	if(!symbol_table || !inst_table || !asm_inst || !machine_inst)
		return ERR_BAD_INPUT;

	int rc;
	memset(machine_inst, '0', WSIZE);
	machine_inst[WSIZE] = '\0';
	if(asm_inst[0] == '@')
	{
		char *symbol = asm_inst + 1;
		size_t address, i, j;
		
		if(strspn(symbol, "0123456789") == strlen(symbol))
			sscanf(symbol, "%zu", &address);
		else
		{
			rc = sym_table_lookup(symbol_table, symbol, &address);	
			if(rc)
				return ERR_LOOKUP_NO_SYM;
		}

		j = WSIZE-1;
		for(i = 0; i < WSIZE - 1; ++i)
			machine_inst[j--] = ((address>>i) & 0x1) ? '1' : '0';
	}
	else
	{
		char *s, *c;
		memcpy(machine_inst, "111", 3);
		ENTRY item, *entry;
		char piece[5];
		s = asm_inst;

		//Interpret destination
		c = strchr(s, '=');
		if(c) 
		{
			piece[0] = 'd';
			sscanf(s, "%3[^=]", piece+1);	//NB: symbols for destination starts with d in the symbol table
			item.key = piece;
			hsearch_r(item, FIND, &entry, inst_table);
			if(!entry)
				return ERR_LOOKUP_NO_DST;

			memcpy(&machine_inst[d1], entry->data, DST_LEN);
			s = c + 1;
		}

		//Interpret computation
		sscanf(s, "%3[^;]", piece);
		item.key = piece;
		hsearch_r(item, FIND, &entry, inst_table);
		if(!entry)
			return ERR_LOOKUP_NO_COMP;
		memcpy(&machine_inst[a], entry->data, COMP_LEN);
		
		c = strchr(s, ';');
		if(c) 
		{
			s = c + 1;
			sscanf(s, "%3s", piece);
			item.key = piece;
			hsearch_r(item, FIND, &entry, inst_table);
			if(!entry)
				return ERR_LOOKUP_NO_JMP;
			memcpy(&machine_inst[j1], entry->data, JMP_LEN);
		}
	}

	return 0;
}


int interpret_asm(FILE* asm_file, const void *symbol_table, FILE* output_file)
{
	if(!asm_file || !symbol_table || !output_file)
		return -1;

	char *line = NULL;
	size_t line_len = 0;
	char parsed_line[SYMBOL_LEN];
	char machine_inst[WSIZE + 1];
	LINE_TYPE line_type;
	int rc = 0;

	struct hsearch_data *instruction_table = calloc(1, sizeof(struct hsearch_data));
	if(!instruction_table)
		return -1;
	
	ENTRY instructions[] = {
		{"0", "0101010"}, {"1", "0111111"}, {"-1", "0111010"}, {"D", "0001100"},
		{"A", "0110000"}, {"!D", "0001101"}, {"!A", "0110001"}, {"-D", "0001111"},
		{"-A", "0110011"}, {"D+1", "0011111"}, {"A+1", "0110111"}, {"D-1", "0001110"},
		{"A-1", "0110010"}, {"D+A", "0000010"}, {"D-A", "0010011"}, {"A-D", "0000111"},
		{"D&A", "0000000"}, {"D|A", "0010101"}, {"M", "1110000"}, {"!M", "1110001"},
		{"-M", "1110011"}, {"M+1", "1110111"}, {"M-1", "1110010"}, {"D+M", "1000010"},
		{"D-M", "1010011"}, {"M-D", "1000111"}, {"D&M", "1000000"}, {"D|M", "1010101"},
		{"JGT", "001"}, {"JEQ", "010"}, {"JGE", "011"}, {"JLT", "100"}, {"JNE", "101"},
		{"JLE", "110"}, {"JMP", "111"}, {"dM", "001"}, {"dD", "010"}, {"dMD", "011"},
		{"dA", "100"}, {"dAM", "101"}, {"dAD", "110"}, {"dAMD", "111"}	
	};

	size_t n_entries = sizeof(instructions) / sizeof(ENTRY);
	hcreate_r(n_entries, instruction_table);
	ENTRY *entry;
	for(size_t i = 0; i < n_entries; ++i)
		hsearch_r(instructions[i], ENTER, &entry, instruction_table);
	
	while(getline(&line, &line_len, asm_file) != -1)
	{
		rc = parse_line(line, parsed_line, &line_type);
		if(rc)
		{
			fprintf(stderr, "interpret_asm: cannot parse '%s'\n", line);
			free(line);
			break;
		}

		if(line_type == A_INST || line_type == C_INST)
		{
			rc = interpret_inst(symbol_table, instruction_table, parsed_line, machine_inst);
			if(rc)
			{
				fprintf(stderr, "interpret_asm: cannot interpret '%s': %d\n", parsed_line, rc);
				free(line);
				break;
			}

			fprintf(output_file, "%s\n", machine_inst);
		}
		free(line);
		line = NULL;
		line_len = 0;
	}
	hdestroy_r(instruction_table);
	return rc;
}

