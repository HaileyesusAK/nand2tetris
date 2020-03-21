#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"
#include "symbol_table.h"

#define BLOCK_COUNT 128
#define N_BUILTIN_SYM 23
#define VAR_BASE 16


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

static int collect_symbols(FILE* file,
					sym_table_entry** resolved_symbols,
					size_t *n_resolved_symbols,
					sym_table_entry** unresolved_symbols,
					size_t *n_unresolved_symbols)
{

	if(!file || !resolved_symbols || !n_resolved_symbols || !unresolved_symbols || !n_unresolved_symbols)
		return -1;

	size_t n_max_resolved = BLOCK_COUNT;	
	size_t n_max_unresolved = BLOCK_COUNT;

	sym_table_entry *resolved = calloc(1, sizeof(sym_table_entry) * n_max_resolved);
	if(!resolved)
		return -2;

	sym_table_entry *unresolved = calloc(1, sizeof(sym_table_entry) * n_max_unresolved);
	if(!unresolved)
	{
		free(resolved);
		return -2;
	}

	void *mem_blk;
	int rc = 0;
	LINE_TYPE line_type;
	char *line = NULL;
	char parsed_line[SYMBOL_LEN];
	size_t pc = 0, i = 0, j = 0, line_len = 0;
	rewind(file);

	while(getline(&line, &line_len, file) != -1)
	{
		line[line_len - 1] = '\0';	

		parse_line(line, parsed_line, &line_type);
		if(line_type == BLANK_LINE)
			continue;
		
		if(line_type == LABEL)
		{
			//reallocate 
			if(i == n_max_resolved)
			{
				n_max_resolved += BLOCK_COUNT; 
				mem_blk= realloc(resolved, n_max_resolved * sizeof(sym_table_entry));
				if(!mem_blk)
				{
					rc = -2;
					break;
				}
				resolved = mem_blk;
			}
			
			char *s = resolved[i].symbol;
			snprintf(s, SYMBOL_LEN, "%s", parsed_line + 1);		//escaping open parenthesis
			s[strlen(s) - 1] = '\0';							//strip away the closing parenthesis

			resolved[i].address = pc;
			i++;
		}
		else if(line_type == A_INST)
		{
			size_t digit_cnt = strspn(parsed_line + 1, "0123456789");
			if(digit_cnt < strlen(parsed_line) - 1)		//A-instruction with symbol
			{
				//reallocate 
				if(j == n_max_unresolved)
				{
					n_max_unresolved += BLOCK_COUNT; 
					mem_blk = realloc(unresolved, n_max_unresolved * sizeof(sym_table_entry));
					if(!mem_blk)
					{
						rc = -2;
						break;
					}
					unresolved = mem_blk;
				}
				
				char *s = unresolved[j].symbol;
				snprintf(s, SYMBOL_LEN, "%s", parsed_line + 1);		//escaping @ character
				j++;
			}
		}
		
		free(line);
		line = NULL;
		line_len = 0;
		if(line_type == A_INST || line_type == C_INST)
			++pc;
	}


	if(rc)
	{
		free(resolved);
		free(unresolved);
	}
	else
	{
		*resolved_symbols = resolved;
		*n_resolved_symbols = i;
		*unresolved_symbols = unresolved;
		*n_unresolved_symbols = j;
	}
	return rc;
}

int build_sym_table(FILE *asm_file, void** symbol_table_ptr)
{
	if(!asm_file || !symbol_table_ptr)
		return -1;

	sym_table_entry *resolved, *unresolved;
	size_t n_resolved, n_unresolved;
	
	sym_table_entry builtin_sym[N_BUILTIN_SYM] = {
		{"R0", 0}, {"R1", 1}, {"R2", 2}, {"R3", 3}, {"R4", 4}, {"R5", 5},
		{"R6", 6}, {"R7", 7}, {"R8", 8}, {"R9", 9}, {"R10", 10}, {"R11", 11},
		{"R12", 12}, {"R13", 13}, {"R14", 14}, {"R15", 15}, {"SP", 0},
		{"LCL", 1}, {"ARG", 2}, {"THIS", 3}, {"THAT", 4}, {"SCREEN", 16384},
		{"KBD", 24576}
	};
	
	int  rc = collect_symbols(asm_file, &resolved, &n_resolved, &unresolved, &n_unresolved);
	if(rc)
	{
		fprintf(stderr, "collect_symbols failed\n");
		return -2;
	}

	size_t n_entries = N_BUILTIN_SYM + n_resolved + n_unresolved;
	void *symbol_table = sym_table_init(n_entries);
	if(!symbol_table)
	{
		fprintf(stderr, "sym_table_init failed\n");
		return -2;
	}

	size_t address;
	for(size_t i = 0; i < N_BUILTIN_SYM; ++i)
	{
		rc = sym_table_insert(symbol_table, &builtin_sym[i], &address);
		if(rc)
		{
			fprintf(stderr, "sym_table_insert failed\n");
			return -2;
		}
	}
	
	for(size_t i = 0; i < n_resolved; ++i)
	{
		rc = sym_table_insert(symbol_table, &resolved[i], &address);
		if(rc)
		{
			fprintf(stderr, "sym_table_insert failed\n");
			return -2;
		}
	}

	size_t n_variables = 0;	
	for(size_t i = 0; i < n_unresolved; ++i)
	{
		rc = sym_table_lookup(symbol_table, unresolved[i].symbol, &address);
		if(rc == -1)
		{
			fprintf(stderr, "sym_table_lookup failed\n");
			return -1;
		}

		if(rc == 1)
		{
			unresolved[i].address = VAR_BASE + n_variables++;
			rc = sym_table_insert(symbol_table, &unresolved[i], &address);
			if(rc)
			{
				fprintf(stderr, "sym_table_insert failed\n");
				return -1;
			}
		}
	}
	
	*symbol_table_ptr = symbol_table;
	free(resolved);
	free(unresolved);
	return 0;
}
