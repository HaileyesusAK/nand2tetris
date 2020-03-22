#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"
#include "symbol_table.h"

#define BLOCK_COUNT 128
#define N_BUILTIN_SYM 23
#define VAR_BASE 16
#define MAX_LINE_LEN 256

static int cmp_entry(void const *e1, void const *e2)
{
	const sym_table_entry *entry1 = (const sym_table_entry*) e1;
	const sym_table_entry *entry2 = (const sym_table_entry*) e2;
	return strcmp(entry1->symbol, entry2->symbol);
}

static int filter_dup_sym(sym_table_entry* dup_list, size_t n_dup_list,
						  sym_table_entry** uniq_list_ptr, size_t* n_uniq_list)
{
	int rc = -1;
	if(!dup_list || !uniq_list_ptr || !n_uniq_list)
		return rc;

	sym_table_entry* uniq_list = calloc(1, n_dup_list * sizeof(sym_table_entry));
	if(!uniq_list)
		return rc;

	size_t n = 0, i = 0;
	qsort(dup_list, n_dup_list, sizeof(sym_table_entry), cmp_entry);

	for(size_t j = 1; j < n_dup_list; ++j)
	{
		if(strcmp(dup_list[i].symbol, dup_list[j].symbol))
		{
			uniq_list[n++]= dup_list[i];
			i = j;
		}
	}
	uniq_list[n++] = dup_list[i];

	void *new_blk = realloc(uniq_list, n * sizeof(sym_table_entry));
	if(!new_blk)
	{
		free(uniq_list);
	}
	else
	{
		*uniq_list_ptr = new_blk;
		*n_uniq_list = n;
		rc = 0;
	}
	return rc;
}



int parse_line(const char* line, char* parsed_line, LINE_TYPE* type)
{
	if(!line || !parsed_line || !type)
		return -1;
	
	*type = BLANK_LINE;
	const char *c = line;
	char *dst;

	*parsed_line='\0';
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
	size_t n_referenced;
	
	sym_table_entry *resolved = calloc(1, sizeof(sym_table_entry) * n_max_resolved);
	if(!resolved)
		return -2;

	sym_table_entry *unresolved = calloc(1, sizeof(sym_table_entry) * n_max_unresolved);
	if(!unresolved)
	{
		free(resolved);
		return -2;
	}
	sym_table_entry *referenced;
	
	void *mem_blk;
	int rc = 0;
	LINE_TYPE line_type;
	char line[MAX_LINE_LEN];
	char parsed_line[MAX_LINE_LEN];
	size_t pc = 0, i = 0, j = 0;
	rewind(file);

	while(fgets(line, MAX_LINE_LEN, file))
	{
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
		
		if(line_type == A_INST || line_type == C_INST)
			++pc;
	}


	//Filter out duplicate entries in the unresolved list
	if(rc)
	{
		free(resolved);
	}
	else
	{
		rc = filter_dup_sym(unresolved, j, &referenced, &n_referenced);
		if(rc)
		{
			free(resolved);
			rc = -2;
		}
		else
		{
			*resolved_symbols = resolved;
			*n_resolved_symbols = i;
			*unresolved_symbols = referenced;
			*n_unresolved_symbols = n_referenced;
		}

	}
	free(unresolved);
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
	*symbol_table_ptr = sym_table_init(n_entries);
	void *symbol_table = *symbol_table_ptr;
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
			return -2;
		}

		if(rc == 1)
		{
			unresolved[i].address = VAR_BASE + n_variables++;
			rc = sym_table_insert(symbol_table, &unresolved[i], &address);
			if(rc)
			{
				fprintf(stderr, "sym_table_insert failed\n");
				return -2;
			}
		}
	}
	
	
	free(resolved);
	free(unresolved);
	return 0;
}
