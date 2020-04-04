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
#define N_VM_INST			29
#define VM_INST_LEN			64

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

typedef size_t Generator(FILE *, ...);


static void *RAM = NULL;
static uint16_t PC;				//Program counter, used when generating instruction for logical operations
static char* static_prefix;

static struct hsearch_data generator_mapping;

static size_t parse_word(const char* src, char* dst, size_t n_dst);
static int parse_vm_inst(const char* vm_inst, char *cmd, InstType* inst_type,
						 char *segment, uint16_t* idx);

static int translate_vm_inst(const char* vm_inst, FILE* asm_inst, size_t* inst_len);

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

void *init_translator(char *prefix)
{
	if(!prefix)
		return NULL;

	static_prefix = strdup(prefix);
	if(!static_prefix)
		return NULL;

	if(!hcreate_r(N_VM_INST, &generator_mapping))
		return NULL;

	RAM = calloc(1, RAM_SIZE);
	if(!RAM)
	{
		hdestroy_r(&generator_mapping);
		free(static_prefix);
		return NULL;
	}

	struct {
		char *cmd;
		void *generator;
	} cmd_generators[N_VM_INST] = { 
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
		{"push_constant", gen_push_const_asm}, {"pop_constant", gen_pop_const_asm},
	};

	size_t i = 0; 
	ENTRY item, *entry;
	for(i = 0; i < N_VM_INST; ++i)
	{
		item.key = cmd_generators[i].cmd;
		item.data = (void*)cmd_generators[i].generator;
		hsearch_r(item, ENTER, &entry, &generator_mapping);
	}
	
	return RAM;
}

void destroy_vm_translator(void* vm_translator)
{
	if(vm_translator)
	{
		free(RAM);
		free(static_prefix);
		hdestroy_r(&generator_mapping);
	}
}

int translate_vm_inst(const char* vm_inst, FILE* asm_inst, size_t* inst_len)
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
			if(!hsearch_r(item, FIND, &entry, &generator_mapping))
				return -2;
			
			Generator *generator = (Generator*)entry->data;
			len = generator(asm_inst, idx);
		}
		break;

		case ALU_INST:
		{
			item.key = cmd;
			if(!hsearch_r(item, FIND, &entry, &generator_mapping))
				return -2;
			
			Generator *generator = (Generator*)entry->data;
			len = generator(asm_inst);
		}
		break;

		default:
			break;
	}

	PC += len;
	*inst_len = len;
	
	return 0;
}

int translate_vm_file(FILE* vm_file, FILE* asm_file)
{
	if(!vm_file || !asm_file)
		return -1;
	
	size_t inst_len;
	int rc;
	PC = 0;
	char vm_inst[VM_INST_LEN];
	while(fgets(vm_inst, VM_INST_LEN, vm_file))
	{
		fprintf(asm_file, "//%s", vm_inst);
		rc = translate_vm_inst(vm_inst, asm_file, &inst_len);
		if(rc)
			return -2;	//TODO: properly handle errors

		PC += inst_len;
	}

	//End of translation
	fprintf(asm_file, "(END)\n"
					  "\t0;JMP\n");

	fprintf(asm_file, "(PUSH_TRUE)\n"
					  "\t@SP\n"
					  "\tA=M\n"
					  "\tM=-1\n"
					  "\t@SP\n"
					  "\tM=M+1\n"
					  "\t@R13\n"
					  "\t0;JMP\n");

	fprintf(asm_file, "(PUSH_FALSE)\n"
					  "\t@SP\n"
					  "\tA=M\n"
					  "\tM=0\n"
					  "\t@SP\n"
					  "\tM=M+1\n"
					  "\t@R13\n"
					  "\t0;JMP\n");
	return 0;
}

size_t parse_word(const char* src, char* dst, size_t n_dst)
{
	size_t i = 0;
	const char *c = src;
	
	while(*c && isblank(*c))
		c++;
	
	while(*c && !isblank(*c) && i < n_dst - 1)
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

	while(*c && isblank(*c))
		c++;	
	
	if(!*c || *c == '/') 
		return 0;

	*inst_type = ALU_INST;
	word_len = parse_word(c, cmd, CMD_LEN);
	if(!strcmp(cmd, "push") || !strcmp(cmd, "pop"))
	{
		*inst_type = MEM_INST;
		word_len = parse_word(c + word_len, segment, SEGMENT_LEN);
		sscanf(c + word_len, "%hu", idx);
	}

	return 0;
}


static size_t gen_binary_alu_op_asm(FILE *asm_file, char op)
{

	size_t len = fprintf(asm_file,
						 "@SP\n"
						 "AM=M-1\n"
						 "D=M\n"
						 "@SP\n"
						 "A=M-1\n");
	if(op == '-')
		len += fprintf(asm_file, "M=M-D\n");
	else
		len += fprintf(asm_file, "M=D%cM\n", op);

	return len;
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
	return fprintf(asm_file, "@SP\nA=M-1\nM=!M\n");
}

static size_t gen_neg_asm(FILE *asm_file)
{
	return fprintf(asm_file, "@SP\nA=M-1\nM=-M\n");
}

static size_t gen_binary_rel_asm(FILE *asm_file, char op)
{

	uint16_t return_addr = PC + 14;

	char *jmp_cmd;
	switch(op)
	{
		case '=': jmp_cmd = "JEQ"; break;
		case '<': jmp_cmd = "JLT"; break;
		case '>': jmp_cmd = "JGT"; break;
	}

	//Save return address on R13
	size_t len = fprintf(asm_file, "@%hu\n"
								   "D=A\n"
								   "@R13\n"
								   "M=D\n",
								   return_addr);

	//Compute x - y and put the result on D
	len += fprintf(asm_file + len, "@SP\n"
								   "AM=M-1\n"
								   "D=M\n"
								   "@SP\n"
								   "AM=M-1\n"
								   "D=M-D\n");
	
	len += fprintf(asm_file + len, "@PUSH_TRUE\n"
								   "D;%s\n"
								   "@PUSH_FALSE\n"
								   "0;JMP\n",
								   jmp_cmd);
	return len;
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
	size_t len = 0;
	char *push_D = "@SP\n"
				   "A=M\n"
				   "M=D\n"
				   "@SP\n"
				   "M=M+1";
					
	if(seg_base == CONSTANT)
	{
		len = fprintf(asm_file, "@%hu\nD=A\n%s\n", idx, push_D);
	}
	else if(seg_base == STATIC)
	{
		len = fprintf(asm_file, "@%s.%hu\nD=M\n%s\n", static_prefix, idx, push_D);
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
				len = fprintf(asm_file,
							  "@%hu\n"
							  "D=A\n"
							  "@%s\n"
							  "A=D+M\n"
							  "D=M\n"
							  "%s\n",
							  idx, symbol, push_D);
				break;
			case POINTER:
			case TEMP:
				len = fprintf(asm_file, "@%hu\nD=M\n%s\n", seg_base, push_D);
				break;
			default:
				break;
		}
	}
	return len;
}

static size_t gen_pop_asm(FILE *asm_file, SegmentBase seg_base, uint16_t idx)
{
	size_t len;
	char *pop_D = "@SP\n"
				  "AM=M-1\n"
				  "D=M";

	if(seg_base == STATIC)
		len = fprintf(asm_file, "%s\n@%s.%hu\nM=D\n", pop_D, static_prefix, idx);
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
				len = fprintf(asm_file,
							  "@%hu\n"
							  "D=A\n"
							  "@%s\n"
							  "M=D+M\n"
							  "%s\n"
							  "@%s\n"
							  "A=M\n"
							  "M=D\n"
							  "@%hu\n"
							  "D=A\n"
							  "@%s\n"
							  "M=M-D\n",
							  idx, symbol, pop_D, symbol, idx, symbol);
				break;
			case TEMP:
			case POINTER:
				len = fprintf(asm_file, "%s\n@%hu\nM=D\n", pop_D, seg_base);
				break;
			default:
				break;
		}
	}
	
	return len;
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
