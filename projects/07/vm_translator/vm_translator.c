#define _GNU_SOURCE
#include <ctype.h>
#include <search.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "vm_translator.h"

#define CMD_LEN				9
#define SEGMENT_LEN			9
#define VM_LINE_LEN			256

typedef enum {
	POINTER = 3, //RAM location 3, coincides with THIS
	TEMP = 5,	//RAM location 5
	LCL,
	ARG,
	THIS,
	THAT,
	CONSTANT,
	STATIC
}SegmentBase;

typedef enum {
	BLANK,
	MEM_INST,
	ALU_INST
}InstType;

typedef struct  {
	size_t n_cmds;
	char **cmds;
	struct hsearch_data* generator_mapping;
	char *static_prefix;
}VmTranslator;

typedef size_t Generator(FILE *, ...);


static uint16_t PC;				//Program counter, used when generating instruction for logical operations
static char* static_prefix;


static size_t parse_word(const char* src, char* dst, size_t n_dst);
static int parse_vm_inst(const char* vm_inst, char *cmd, InstType* inst_type,
						 char *segment, uint16_t* idx);

static int translate_vm_inst(const VmTranslator* vm_translator,
							 const char* vm_inst, FILE* asm_inst, size_t* inst_len);

static size_t gen_add_asm(FILE *asm_file);
static size_t gen_and_asm(FILE *asm_file);
static size_t gen_eq_asm(FILE *asm_file);
static size_t gen_gt_asm(FILE *asm_file);
static size_t gen_lt_asm(FILE *asm_file);
static size_t gen_neg_asm(FILE *asm_file);
static size_t gen_not_asm(FILE *asm_file);
static size_t gen_or_asm(FILE *asm_file);
static size_t gen_sub_asm(FILE *asm_file);
static size_t gen_binary_rel_asm(FILE *asm_file, char op);
static size_t gen_binary_alu_op_asm(FILE *asm_file, char op);

static size_t gen_pop_asm(FILE *asm_file, SegmentBase seg_base, uint16_t idx);
static size_t gen_push_asm(FILE *asm_file, SegmentBase seg_base, uint16_t idx);
static size_t gen_push_lcl_asm(FILE *asm_file, uint16_t idx);
static size_t gen_pop_lcl_asm(FILE *asm_file, uint16_t idx);
static size_t gen_push_arg_asm(FILE *asm_file, uint16_t idx);
static size_t gen_pop_arg_asm(FILE *asm_file, uint16_t idx);
static size_t gen_push_static_asm(FILE *asm_file, uint16_t idx);
static size_t gen_pop_static_asm(FILE *asm_file, uint16_t idx);
static size_t gen_push_ptr_asm(FILE *asm_file, uint16_t idx);
static size_t gen_pop_ptr_asm(FILE *asm_file, uint16_t idx);
static size_t gen_push_this_asm(FILE *asm_file, uint16_t idx);
static size_t gen_pop_this_asm(FILE *asm_file, uint16_t idx);
static size_t gen_push_that_asm(FILE *asm_file, uint16_t idx);
static size_t gen_pop_that_asm(FILE *asm_file, uint16_t idx);
static size_t gen_push_temp_asm(FILE *asm_file, uint16_t idx);
static size_t gen_pop_temp_asm(FILE *asm_file, uint16_t idx);
static size_t gen_push_const_asm(FILE *asm_file, uint16_t idx);
static size_t gen_pop_const_asm(FILE *asm_file, uint16_t idx);

void *init_vm_translator(char *prefix)
{

	if(!prefix)
	{
		fprintf(stderr, "NULL input pointer at %s, line %d\n", __FILE__, __LINE__);
		return NULL;
	}

	VmTranslator* vm_translator = calloc(1, sizeof(VmTranslator));
	if(!vm_translator)
	{
		fprintf(stderr, "calloc failed at %s, line %d\n", __FILE__, __LINE__);
		return NULL;
	}
	
	vm_translator->generator_mapping = calloc(1, sizeof(struct hsearch_data));
	if(!vm_translator->generator_mapping)
	{
		fprintf(stderr, "calloc failed at %s, line %d\n", __FILE__, __LINE__);
		destroy_vm_translator(vm_translator);
		return NULL;
	}

	vm_translator->static_prefix = strdup(prefix);
	static_prefix = vm_translator->static_prefix;

	if(!vm_translator->static_prefix)
	{
		fprintf(stderr, "strdup failed at %s, line %d\n", __FILE__, __LINE__);
		destroy_vm_translator(vm_translator);
		return NULL;
	}
	
	struct CmdGen{
		char *cmd;
		void *generator;
	} cmd_generators[] = { 
		{"add",  gen_add_asm}, {"sub", gen_sub_asm}, {"neg", gen_neg_asm},
		{"eq", gen_eq_asm}, {"gt", gen_gt_asm}, {"lt", gen_lt_asm},
		{"and", gen_and_asm}, {"or", gen_or_asm}, {"not", gen_not_asm},
		{"push_local", gen_push_lcl_asm}, {"pop_local", gen_pop_lcl_asm},
		{"push_argument", gen_push_arg_asm}, {"pop_argument", gen_pop_arg_asm},
		{"push_this", gen_push_this_asm}, {"pop_this", gen_pop_this_asm},
		{"push_that", gen_push_that_asm}, {"pop_that", gen_pop_that_asm},
		{"push_pointer", gen_push_ptr_asm}, {"pop_pointer", gen_pop_ptr_asm},
		{"push_temp", gen_push_temp_asm}, {"pop_temp", gen_pop_temp_asm},
		{"push_static", gen_push_static_asm}, {"pop_static", gen_pop_static_asm},
		{"push_constant", gen_push_const_asm}, {"pop_constant", gen_pop_const_asm}
	};

	vm_translator->n_cmds = sizeof(cmd_generators) / sizeof(struct CmdGen);	
	vm_translator->cmds = calloc(1, vm_translator->n_cmds * sizeof(char*));
	if(!vm_translator->cmds)
	{
		fprintf(stderr, "calloc failed at %s, line %d\n", __FILE__, __LINE__);
		destroy_vm_translator(vm_translator);
		return NULL;
	}
	
	for(size_t i = 0; i < vm_translator->n_cmds; ++i)
	{
		vm_translator->cmds[i] = strdup(cmd_generators[i].cmd);
		if(!vm_translator->cmds[i])
		{
			fprintf(stderr, "strdup failed at %s, line %d\n", __FILE__, __LINE__);
			destroy_vm_translator(vm_translator);
			return NULL;
		}
	}
	
	if(!hcreate_r(vm_translator->n_cmds, vm_translator->generator_mapping))
	{
		fprintf(stderr, "hcreate_r failed at %s, line %d\n", __FILE__, __LINE__);
		destroy_vm_translator(vm_translator);
		return NULL;
	}

	size_t i = 0; 
	ENTRY item, *entry;
	for(i = 0; i < vm_translator->n_cmds; ++i)
	{
		item.key = vm_translator->cmds[i];
		item.data = cmd_generators[i].generator;
		if(!hsearch_r(item, ENTER, &entry, vm_translator->generator_mapping))
		{
			fprintf(stderr, "hsearch_r failed at %s, line %d\n", __FILE__, __LINE__);
			destroy_vm_translator(vm_translator);
			return NULL;
		}
	}
	
	return vm_translator;
}

void destroy_vm_translator(void* handle)
{
	VmTranslator *vm_translator = (VmTranslator *)handle;
	if(vm_translator)
	{
		if(vm_translator->static_prefix)
			free(vm_translator->static_prefix);
		
		if(vm_translator->cmds)
		{
			for(size_t i = 0; i < vm_translator->n_cmds; ++i)
				if(vm_translator->cmds[i])
					free(vm_translator->cmds[i]);
			free(vm_translator->cmds);
		}
		
		if(vm_translator->generator_mapping)
		{
			hdestroy_r(vm_translator->generator_mapping);
			free(vm_translator->generator_mapping);
		}
		
		free(vm_translator);
	}
}

int translate_vm_inst(const VmTranslator* vm_translator, const char* vm_inst, FILE* asm_inst, size_t* inst_len)
{
	char cmd[CMD_LEN];
	char segment[SEGMENT_LEN];
	uint16_t idx;
	InstType inst_type;
	ENTRY item, *entry;
	size_t len = 0;

	parse_vm_inst(vm_inst, cmd, &inst_type, segment, &idx);

	switch(inst_type)
	{
		case MEM_INST:
		{
			char key[CMD_LEN + SEGMENT_LEN + 1];
			snprintf(key, CMD_LEN + SEGMENT_LEN + 1, "%s_%s", cmd, segment);
			item.key = key;
			if(!hsearch_r(item, FIND, &entry, vm_translator->generator_mapping))
			{
				fprintf(stderr, "Cannot translate VM command '%s %s' at %s, line %d\n",
								cmd, segment, __FILE__, __LINE__);
				return -1;
			}	

			Generator *generator = (Generator*)entry->data;
			len = generator(asm_inst, idx);
		}
		break;

		case ALU_INST:
		{
			item.key = cmd;
			if(!hsearch_r(item, FIND, &entry, vm_translator->generator_mapping))
			{
				fprintf(stderr, "Cannot translate VM command '%s' at %s, line %d\n",
								cmd, __FILE__, __LINE__);
				return -1;
			}

			Generator *generator = (Generator*)entry->data;
			len = generator(asm_inst);
		}
		break;

		default:
			break;
	}

	*inst_len = len;
	
	return 0;
}

int translate_vm_file(const void* handle, FILE* vm_file, FILE* asm_file)
{
	if(!handle || !vm_file || !asm_file)
	{
		fprintf(stderr, "NULL input pointer at %s, line %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	size_t inst_len;
	char vm_inst[VM_LINE_LEN];
	VmTranslator* vm_translator = (VmTranslator*) handle;

	fprintf(asm_file, "\t@BEGIN\n"
					  "\t0;JMP\n\n");

	fprintf(asm_file, "(PUSH_TRUE)\n"
					  "\t@SP\n"
					  "\tA=M\n"
					  "\tM=-1\n"
					  "\t@SP\n"
					  "\tM=M+1\n"
					  "\t@R13\n"
					  "\tA=M\n"
					  "\t0;JMP\n\n");

	fprintf(asm_file, "(PUSH_FALSE)\n"
					  "\t@SP\n"
					  "\tA=M\n"
					  "\tM=0\n"
					  "\t@SP\n"
					  "\tM=M+1\n"
					  "\t@R13\n"
					  "\tA=M\n"
					  "\t0;JMP\n\n");

	fprintf(asm_file, "(BEGIN)\n");
	const size_t N_BEG_LINES = 18;
	PC = N_BEG_LINES;	//The line number of the next assembly instruction
	while(fgets(vm_inst, VM_LINE_LEN, vm_file))
	{
		//vm_inst[strlen(vm_inst)-1] = '\0';
		int len = fprintf(asm_file, "//%s", vm_inst);
		int rc = translate_vm_inst(vm_translator, vm_inst, asm_file, &inst_len);
		if(rc)
		{
			fprintf(stderr, "Cannot translate VM instruction '%s' at %s, line %d\n",
							vm_inst, __FILE__, __LINE__);
			return -1;	
		}

		//Overwrite comment if no instruction is translated
		if(!inst_len)
			fseek(asm_file, -len, SEEK_CUR); 
		else
			fprintf(asm_file, "\n");
				
		PC += inst_len;
	}

	//End of translation
	fprintf(asm_file, "(END)\n"
					  "\t@END\n"
					  "\t0;JMP\n\n");

	return 0;
}

size_t parse_word(const char* src, char* dst, size_t n_dst)
{
	size_t i = 0;
	const char *c = src;
	
	while(*c && isblank(*c))
		c++;
	
	while(*c && !isblank(*c) && !isspace(*c) && i < n_dst - 1)
		dst[i++] = *c++;

	dst[i] = '\0';

	return i;
}

int parse_vm_inst(const char* vm_inst, char *cmd, InstType* inst_type,
						 char *segment, uint16_t* idx)
{
	const char *c = vm_inst;
	size_t word_len;	
	*inst_type = BLANK;

	while(*c && (isblank(*c) || isspace(*c)))
		c++;	
	
	if(!*c || *c == '/') 
		return 0;

	*inst_type = ALU_INST;
	
	word_len = parse_word(c, cmd, CMD_LEN);
	if(!strcmp(cmd, "push") || !strcmp(cmd, "pop"))
	{
		*inst_type = MEM_INST;
		sscanf(c + word_len, "%s %hu", segment, idx);
	}

	return 0;
}


static size_t gen_binary_alu_op_asm(FILE *asm_file, char op)
{

	size_t n_lines = 6;
	fprintf(asm_file,
			"\t@SP\n"
			"\tAM=M-1\n"
			"\tD=M\n"
			"\t@SP\n"
			"\tA=M-1\n");
	
	if(op == '-')
		fprintf(asm_file, "\tM=M-D\n");
	else
		fprintf(asm_file, "\tM=D%cM\n", op);

	return n_lines;
}

static size_t gen_add_asm(FILE *asm_file)
{
	return gen_binary_alu_op_asm(asm_file, '+');
}

static size_t gen_sub_asm(FILE *asm_file)
{
	return gen_binary_alu_op_asm(asm_file, '-');
}

static size_t gen_or_asm(FILE *asm_file)
{
	return gen_binary_alu_op_asm(asm_file, '|');
}

static size_t gen_and_asm(FILE *asm_file)
{
	return gen_binary_alu_op_asm(asm_file, '&');
}

static size_t gen_not_asm(FILE *asm_file)
{
	size_t n_lines = 3;
	fprintf(asm_file, 
			"\t@SP\n"
			"\tA=M-1\n"
			"\tM=!M\n");
	
	return n_lines;
}

static size_t gen_neg_asm(FILE *asm_file)
{
	size_t n_lines = 3;
	fprintf(asm_file,
		    "\t@SP\n"
		    "\tA=M-1\n"
		    "\tM=-M\n");
	
	return n_lines;
}

static size_t gen_binary_rel_asm(FILE *asm_file, char op)
{

	size_t n_lines =  14;
	uint16_t return_addr = PC + n_lines;
	char *jmp_cmd;
	
	switch(op)
	{
		case '=': jmp_cmd = "JEQ"; break;
		case '<': jmp_cmd = "JLT"; break;
		case '>': jmp_cmd = "JGT"; break;
	}

	//Save return address on R13
	fprintf(asm_file, "\t@%hu\n"
				      "\tD=A\n"
				      "\t@R13\n"
				      "\tM=D\n",
				      return_addr);

	//Compute x - y and put the result on D
	fprintf(asm_file, "\t@SP\n"
					  "\tAM=M-1\n"
					  "\tD=M\n"
					  "\t@SP\n"
					  "\tAM=M-1\n"
					  "\tD=M-D\n");
	
	fprintf(asm_file, "\t@PUSH_TRUE\n"
					  "\tD;%s\n"
					  "\t@PUSH_FALSE\n"
					  "\t0;JMP\n",
					  jmp_cmd);
	return n_lines;
}

static size_t gen_eq_asm(FILE *asm_file)
{
	return gen_binary_rel_asm(asm_file, '=');
}

static size_t gen_lt_asm(FILE *asm_file)
{
	return gen_binary_rel_asm(asm_file, '<');
}

static size_t gen_gt_asm(FILE *asm_file)
{
	return gen_binary_rel_asm(asm_file, '>');
}

static size_t gen_push_asm(FILE *asm_file, SegmentBase seg_base, uint16_t idx)
{
	size_t n_lines = 5;
	char *push_D = "@SP\n"
				   "\tA=M\n"
				   "\tM=D\n"
				   "\t@SP\n"
				   "\tM=M+1";
					
	if(seg_base == CONSTANT)
	{
		fprintf(asm_file,
				"\t@%hu\n"
				"\tD=A\n"
				"\t%s\n", idx, push_D);
		n_lines += 2;
	}
	else if(seg_base == STATIC)
	{
		fprintf(asm_file,
			    "\t@%s.%hu\n"
			    "\tD=M\n"
			    "\t%s\n", static_prefix, idx, push_D);
		n_lines += 2;
	}
	else
	{
		char *symbol;
		switch(seg_base)
		{
			case LCL: symbol = "LCL"; break;
			case ARG: symbol = "ARG"; break;
			case THIS: symbol = "THIS"; break;
			case THAT: symbol = "THAT"; break;
			default: break;
		}
		
		switch(seg_base)
		{
			case LCL:
			case ARG:
			case THIS:
			case THAT:
				n_lines += 5;
				fprintf(asm_file,
						"\t@%hu\n"
						"\tD=A\n"
						"\t@%s\n"
						"\tA=D+M\n"
						"\tD=M\n"
						"\t%s\n",
						idx, symbol, push_D);
				break;
			case POINTER:
			case TEMP:
				n_lines += 2;
				fprintf(asm_file,
						"\t@%hu\n"
						"\tD=M\n"
						"\t%s\n", seg_base + idx, push_D);
				break;
			default:
				break;
		}
	}
	return n_lines;
}

static size_t gen_pop_asm(FILE *asm_file, SegmentBase seg_base, uint16_t idx)
{
	char *pop_D = "@SP\n"
				  "\tAM=M-1\n"
				  "\tD=M";
	
	size_t n_lines = 3;

	if(seg_base == STATIC)
	{
		fprintf(asm_file,
				"\t%s\n"
				"\t@%s.%hu\n"
				"\tM=D\n",
				pop_D, static_prefix, idx);
		n_lines += 2;
	}
	else
	{
	
		char *symbol;
		switch(seg_base)
		{
			case LCL: symbol = "LCL"; break;
			case ARG: symbol = "ARG"; break;
			case THIS: symbol = "THIS"; break;
			case THAT: symbol = "THAT"; break;
			default: break;
		}

		switch(seg_base)
		{
			case LCL:
			case ARG:
			case THIS:
			case THAT:
				fprintf(asm_file,
						"\t@%hu\n"
						"\tD=A\n"
						"\t@%s\n"
						"\tM=D+M\n"
						"\t%s\n"
						"\t@%s\n"
						"\tA=M\n"
						"\tM=D\n"
						"\t@%hu\n"
						"\tD=A\n"
						"\t@%s\n"
						"\tM=M-D\n",
						idx, symbol, pop_D, symbol, idx, symbol);
				n_lines += 11;
				break;
			case TEMP:
			case POINTER:
				fprintf(asm_file,
						"\t%s\n"
						"\t@%hu\n"
						"\tM=D\n", pop_D, seg_base + idx);
				n_lines += 2;
				break;
			default:
				break;
		}
	}
	
	return n_lines;
}

static size_t gen_push_lcl_asm(FILE *asm_file, uint16_t idx)
{
	return gen_push_asm(asm_file, LCL, idx);
}

static size_t gen_pop_lcl_asm(FILE *asm_file, uint16_t idx)
{
	return gen_pop_asm(asm_file, LCL, idx);
}

static size_t gen_push_arg_asm(FILE *asm_file, uint16_t idx)
{
	return gen_push_asm(asm_file, ARG, idx);
}

static size_t gen_pop_arg_asm(FILE *asm_file, uint16_t idx)
{
	return gen_pop_asm(asm_file, ARG, idx);
}

static size_t gen_push_static_asm(FILE *asm_file, uint16_t idx)
{
	return gen_push_asm(asm_file, STATIC, idx);
}

static size_t gen_pop_static_asm(FILE *asm_file, uint16_t idx)
{
	return gen_pop_asm(asm_file, STATIC, idx);
}

static size_t gen_push_ptr_asm(FILE *asm_file, uint16_t idx)
{
	return gen_push_asm(asm_file, POINTER, idx);
}

static size_t gen_pop_ptr_asm(FILE *asm_file, uint16_t idx)
{
	return gen_pop_asm(asm_file, POINTER, idx);
}

static size_t gen_push_this_asm(FILE *asm_file, uint16_t idx)
{
	return gen_push_asm(asm_file, THIS, idx);
}

static size_t gen_pop_this_asm(FILE *asm_file, uint16_t idx)
{
	return gen_pop_asm(asm_file, THIS, idx);
}

static size_t gen_push_that_asm(FILE *asm_file, uint16_t idx)
{
	return gen_push_asm(asm_file, THAT, idx);
}

static size_t gen_pop_that_asm(FILE *asm_file, uint16_t idx)
{
	return gen_pop_asm(asm_file, THAT, idx);
}

static size_t gen_push_temp_asm(FILE *asm_file, uint16_t idx)
{
	return gen_push_asm(asm_file, TEMP, idx);
}

static size_t gen_pop_temp_asm(FILE *asm_file, uint16_t idx)
{
	return gen_pop_asm(asm_file, TEMP, idx);
}

static size_t gen_push_const_asm(FILE *asm_file, uint16_t idx)
{
	return gen_push_asm(asm_file, CONSTANT, idx);
}

static size_t gen_pop_const_asm(FILE *asm_file, uint16_t idx)
{
	return gen_pop_asm(asm_file, CONSTANT, 0);
}
